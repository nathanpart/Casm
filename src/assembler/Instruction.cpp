//
// Created by nathan on 7/27/20.
//


#include <Cpu.h>
#include "../Parser/token.h"
#include "../Parser/graminit.h"
#include "Instruction.h"
#include "PseudoOp.h"
#include "Line.h"
#include "Variable.h"
#include "Region.h"
#include "MacroLine.h"
#include "IncludeLine.h"
#include "AddressMode.h"
#include "Error.h"


using namespace std;

void Instruction::createInstruction(node &inst_node, Line &asm_line) {
    node &child = inst_node.child.front();
    Expression expression;
    auto instr = make_shared<Instruction>(*asm_line.state);
    switch (inst_node.type) {
        case op_code:
            asm_line.lineType = LineTypes::cpu;
            switch (child.type) {
                case GROUP1_INST:
                    createGroup1(child, asm_line);
                    break;
                case GROUP2_INST:
                    createGroup2(child, asm_line);
                    break;
                case GROUP3_INST:
                    createGroup3(child, asm_line);
                    break;
                case GROUP4_INST:
                    createGroup4(child, asm_line);
                    break;
                case GROUP5_INST:
                    createGroup5(child, asm_line);
                    break;
                case GROUP6_INST:
                    createGroup6(child, asm_line);
                    break;
                case GROUP7_INST:
                    createGroup7(child, asm_line);
                    break;
                case GROUP8_INST:
                    createGroup8(child, asm_line);
                    break;
                case GROUP9_INST:
                    createGroup9(child, asm_line);
                    break;
                case GROUP10_INST:
                    createGroup10(child, asm_line);
                    break;
                case GROUP11_INST:
                    createGroup11(child, asm_line);
                    break;
                case GROUP12_INST:
                    createGroup12(child, asm_line);
                    break;
                case GROUP13_INST:
                    createGroup13(child, asm_line);
                    break;
                case GROUP14_INST:
                    createGroup14(child, asm_line);
                    break;
                case GROUP15_INST:
                    createGroup15(child, asm_line);
                    break;
                case GROUP16_INST:
                    createGroup16(child, asm_line);
                    break;
            }
            break;

        case pseudo_op:
            asm_line.lineType = LineTypes::pseudo_op;
            PseudoOp::createInstruction(inst_node, asm_line);
            break;

        case variable:
            asm_line.lineType = LineTypes::variable;
            asm_line.instruction = unique_ptr<Instruction>(new Variable(*asm_line.state));
            expression.buildRpnList(inst_node.child.back(), asm_line.lineText);
            asm_line.expressionList.push_back({inst_node.child.back().location, expression});
            break;

        case region:
            asm_line.lineType = LineTypes::segment;
            Region::createInstruction(inst_node, asm_line);
            break;

        case macro:
            asm_line.lineType = LineTypes::macro;
            MacroLine::createInstruction(inst_node, asm_line);
            break;

        case include:
            asm_line.lineType = LineTypes::include;
            IncludeLine::createInstruction(inst_node, asm_line);
            break;

        case external:
            asm_line.lineType = LineTypes::external;
            if (child.type == IMPORT) {
                instr->isImport = true;
            } else {
                instr->isImport = false;
            }
            node &ext_addr = inst_node.child.back();
            for (auto &addr_item: ext_addr.child) {
                if (addr_item.type == DOT) {
                    instr->hasDot = true;
                } else {
                    if (instr->hasDot) {
                        instr->name2 = addr_item.str;
                    }
                    else {
                        instr->name1 = addr_item.str;
                    }
                }
            }
            asm_line.instruction = instr;
    }
}

void Instruction::pass1() {
    auto asm_line = state.getCurrentLine();
    auto segment = state.getCurrentSegment();

    if (asm_line->lineType == LineTypes::cpu) {

        if (cpus.count(state.cpuType) == 0) {
            throw CasmErrorException("The currently selected cpu does not support this instruction",
                                     asm_line->instructionLoc, asm_line->lineText);
        }
        uint8_t cpu_op;
        set<CpuType> cpu_type;

        auto addr_mode_rec = opCodes->at(asm_line->addressMode);
        tie(cpu_op, cpu_type) = addr_mode_rec;
        if (cpu_type.count(state.cpuType) == 0) {
            throw CasmErrorException("The currently selected cpu does not support this address mode.",
                                     asm_line->instructionLoc, asm_line->lineText);
        }
        opCode = cpu_op;
        if (asm_line->hasLabel()) {
            segment->defineLabel(asm_line->label);
        }
        state.allocateSpace(getAddressModeSize(asm_line->addressMode, state, immType) + 1);
    }
    else if (asm_line->lineType == LineTypes::external) {
        if (isImport) {
            if (asm_line->label.empty()) {
                if (!name1.empty() || !hasDot) {
                    state.importSymbol(name1, name1, name2);
                }
                else {
                    state.importSymbol(name2, name1, name2);
                }
            }
            else {
                if (!asm_line->normalLabel()) {
                    throw CasmErrorException("Label cannot be local or variable.", asm_line->labelLoc,
                                             asm_line->lineText);
                }
                state.importSymbol(asm_line->label, name1, name2);
            }
        }
    }
}

void Instruction::pass2() {
    auto asm_line = state.getCurrentLine();

    if (asm_line->lineType == LineTypes::cpu) {
        state.storeByte(opCode);
        writeOperand(asm_line->addressMode, state, immType);
    }
    else if (asm_line->lineType == LineTypes::external && !isImport){
        if (hasDot) {
            throw CasmErrorException("Cannot export reference to another segment.", asm_line->instructionLoc,
                                     asm_line->lineText);
        }
        if (asm_line->hasLabel() && !asm_line->normalLabel()) {
            throw CasmErrorException("Label cannot be local or variable.", asm_line->labelLoc,
                                     asm_line->lineText);
        }
        state.exportSymbol(name1, asm_line->hasLabel() ? asm_line->label : name1);
    }
}

void Instruction::setInstruction(const string &inst_str) {
    for (auto& instr: cpuInstrMap) {
        auto& instr_rec = instr.second;
        instruction = instr.first;
        opCodes = &get<0>(instr_rec);
        cpus = get<1>(instr_rec);
        auto re = get<2>(instr_rec);
        immType = get<3>(instr_rec);
        smatch results;

        if (regex_match(inst_str, results, re)) {
            break;
        }
    }
}

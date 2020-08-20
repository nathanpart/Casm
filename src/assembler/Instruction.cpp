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
            asm_line.instruction = unique_ptr<Instruction>(new Variable);
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

    }

}

void Instruction::pass1(Line &asm_line, AsmState &state) {
    if (asm_line.lineType != LineTypes::cpu) return;

    if (cpus.count(state.cpuType) == 0) {
        throw CasmErrorException("The currently selected cpu does not support this instruction",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    uint8_t cpu_op;
    set<CpuType> cpu_type;

    auto addr_mode_rec = opCodes->at(asm_line.addressMode);
    tie(cpu_op, cpu_type) = addr_mode_rec;
    if (cpu_type.count(state.cpuType) == 0) {
        throw CasmErrorException("The currently selected cpu does not support this address mode.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    opCode = cpu_op;
    state.defineLabel();
    state.allocateSpace(getAddressModeSize(asm_line.addressMode, state, immType) + 1);
}

void Instruction::pass2(Line & asm_line, AsmState & state) {
    state.storeByte(opCode);
    writeOperand(asm_line.addressMode, state, immType);
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

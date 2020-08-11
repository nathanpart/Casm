//
// Created by nathan on 7/27/20.
//


#include <instructions/Group1.h>
#include <instructions/Group2.h>
#include <instructions/Group4.h>
#include <instructions/Group3.h>
#include <instructions/Group5.h>
#include <instructions/Group6.h>
#include <instructions/Group7.h>
#include <instructions/Group8.h>
#include <instructions/Group9.h>
#include <instructions/Group10.h>
#include <instructions/Group11.h>
#include <instructions/Group12.h>
#include <instructions/Group13.h>
#include <instructions/Group14.h>
#include <instructions/Group15.h>
#include <instructions/Group16.h>
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
                    Group1::createInstruction(child, asm_line);
                    break;
                case GROUP2_INST:
                    Group2::createInstruction(child, asm_line);
                    break;
                case GROUP3_INST:
                    Group3::createInstruction(child, asm_line);
                    break;
                case GROUP4_INST:
                    Group4::createInstruction(child, asm_line);
                    break;
                case GROUP5_INST:
                    Group5::createInstruction(child, asm_line);
                    break;
                case GROUP6_INST:
                    Group6::createInstruction(child, asm_line);
                    break;
                case GROUP7_INST:
                    Group7::createInstruction(child, asm_line);
                    break;
                case GROUP8_INST:
                    Group8::createInstruction(child, asm_line);
                    break;
                case GROUP9_INST:
                    Group9::createInstruction(child, asm_line);
                    break;
                case GROUP10_INST:
                    Group10::createInstruction(child, asm_line);
                    break;
                case GROUP11_INST:
                    Group11::createInstruction(child, asm_line);
                    break;
                case GROUP12_INST:
                    Group12::createInstruction(child, asm_line);
                    break;
                case GROUP13_INST:
                    Group13::createInstruction(child, asm_line);
                    break;
                case GROUP14_INST:
                    Group14::createInstruction(child, asm_line);
                    break;
                case GROUP15_INST:
                    Group15::createInstruction(child, asm_line);
                    break;
                case GROUP16_INST:
                    Group16::createInstruction(child, asm_line);
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
    if (asm_line.lineType == LineTypes::cpu) {
        state.defineLabel();
        state.allocateSpace(getAddressModeSize(asm_line.addressMode, state, true) + 1);
    }
    if (!hasAddressMode(state)) {
        throw CasmErrorException("The currently selected cpu does not support this address mode.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
}

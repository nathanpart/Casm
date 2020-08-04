//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group2.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"

using namespace std;

void Group2::createInstruction(node &group_node, Line &asm_line) {
    bool hasOperand = false;
    bool hasAreg = false;
    bool hasXreg = false;
    Expression expression;
    string inst;

    for (const auto& child_node: group_node.child) {
        switch (group_node.type) {
            case AREG:
                hasAreg = true;
                hasOperand = true;
                break;
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case exp:
                hasOperand = true;
                expression.buildRpnList(group_node);
                asm_line.expressionList.push_back(expression);
                break;
            case x_index:
                hasOperand = true;
                hasXreg = true;
                break;
            case GROUP2_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "ASL") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstAsl());
                }
                if (inst == "DEC") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstDec());
                }
                if (inst == "INC") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstInc());
                }
                if (inst == "LSR") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstLsr());
                }
                if (inst == "ROL") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstRol());
                }
                if (inst == "ROR") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstRor());
                }
        }
        if (!hasOperand || hasAreg) {
            asm_line.addressMode = AddressModes::acc;
        } else {
            if (asm_line.hasDpZp) {
                asm_line.addressMode = hasXreg ? AddressModes::dp_x : AddressModes::dp;
            }
            else {
                asm_line.addressMode = hasXreg ? AddressModes::abs_x : AddressModes::abs;
            }
        }

    }




}

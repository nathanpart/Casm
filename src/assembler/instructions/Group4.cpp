//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group4.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;

void Group4::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;
    bool hasXIndex = false;

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case imm_exp:
                for (const auto& imm_node: child_node.child) {
                    switch (imm_node.type) {
                        case WIDE:
                            asm_line.hasWide = true;
                            break;
                        case SHORT:
                            asm_line.hasShort = true;
                            break;
                        case exp:
                            expression.buildRpnList(imm_node, asm_line.lineText);
                            asm_line.expressionList.push_back({imm_node.location, expression});
                            break;
                    }
                }
                asm_line.addressMode = AddressModes::imm;
                break;

            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;

            case exp:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;

            case x_index:
                hasXIndex = true;
                break;

            case GROUP4_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "BIT") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBit());
                }
                if (inst == "LDY") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstLdy());
                }
        }
    }
    if (asm_line.addressMode != AddressModes::imm) {
        if (asm_line.hasDpZp) {
            asm_line.addressMode = hasXIndex ? AddressModes::dp_x : AddressModes::dp;
        }
        else {
            asm_line.addressMode = hasXIndex ? AddressModes::abs_x : AddressModes::abs;
        }
    }
}

void InstLdy::pass1(Line &asm_line, AsmState &state) {
    state.defineLabel();
    state.allocateSpace(getAddressModeSize(asm_line.addressMode, state, false));
}

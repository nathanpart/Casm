//
// Created by nathan on 7/30/20.
//


#include "Group11.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;

void Group11::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasYIndex = false;

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case imm_exp:
                asm_line.addressMode = AddressModes::imm;
                for (const auto &imm_node: child_node.child) {
                    switch (imm_node.type) {
                        case WIDE:
                            asm_line.hasWide = true;
                            break;
                        case SHORT:
                            asm_line.hasShort = true;
                            break;
                        case exp:
                            expression.buildRpnList(imm_node);
                            break;
                        default:;  // Don't need to do anything with the hash
                    }
                }
                break;
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case exp:
                expression.buildRpnList(child_node);
                break;
            case y_index:
                hasYIndex = true;
                break;

        }
    }
    asm_line.expressionList.push_back(expression);
    asm_line.instruction = unique_ptr<Instruction>(new Group11);
    if (asm_line.addressMode != AddressModes::imm) {
        if (asm_line.hasDpZp) {
            asm_line.addressMode = hasYIndex ? AddressModes::dp_y : AddressModes::dp;
        }
        else {
            asm_line.addressMode = hasYIndex ? AddressModes::abs_y: AddressModes::abs;
        }
    }
}

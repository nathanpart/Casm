//
// Created by nathan on 7/30/20.
//

#include "Group12.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;

void Group12::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasYIndex = false;

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case exp:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;
            case y_index:
                hasYIndex = true;
                break;

        }
    }
    asm_line.instruction = unique_ptr<Instruction>(new Group12);
    if (asm_line.hasDpZp) {
        asm_line.addressMode = hasYIndex ? AddressModes::dp_y : AddressModes::dp;
    }
    else {
        asm_line.addressMode = hasYIndex ? AddressModes::abs_y: AddressModes::abs;
    }
}

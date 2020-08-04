//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group8.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;


void Group8::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool isIndirect;
    bool hasXIndex;
    bool hasSIndex;

    for (auto& child_node: group_node.child) {
        node *exp_node = &child_node;
        switch (child_node.type) {
            case LONG:
                asm_line.hasLong = true;
                break;

            case exp:
                isIndirect = isExpTreeIndirect(exp_node, hasXIndex, hasSIndex);
                expression.buildRpnList(*exp_node);
                asm_line.expressionList.push_back(expression);
                break;

            case jmp_long_ind_exp:
                isIndirect = true;
                asm_line.hasLong = true;
                expression.buildRpnList(child_node.child[1]);
                asm_line.expressionList.push_back(expression);
                break;
        }
    }
    asm_line.instruction = unique_ptr<Instruction>(new Group8);

    if (isIndirect) {
        if (asm_line.hasLong) {
            asm_line.addressMode = AddressModes::abs_ind;
        }
        else
            asm_line.addressMode = hasXIndex ? AddressModes::abs_ind_x : AddressModes::abs_ind;
    }
    else {
        asm_line.addressMode = asm_line.hasLong ? AddressModes::abs_long : AddressModes::abs;
    }
}

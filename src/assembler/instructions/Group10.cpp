//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group10.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;

void Group10::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasXIndex = false;
    bool hasSIndex = false;
    bool isIndirect = false;
    string inst;


    for (auto& child_node: group_node.child) {
        node *exp_node = &child_node;
        switch (child_node.type) {
            case LONG:
                asm_line.hasLong = true;
                break;
            case exp:
                isIndirect = isExpTreeIndirect(exp_node, hasXIndex, hasSIndex);
                expression.buildRpnList(*exp_node);
                if (isIndirect && (!hasXIndex || hasSIndex)) {
                    throw CasmErrorException("Bad indirect: JSR only supports Absolute Indexed Indirect (exp,x).",
                                             child_node.lineno, child_node.col_offset);
                }
                asm_line.expressionList.push_back(expression);
                break;
        }
    }
    if (isIndirect) {
        if (asm_line.hasLong) {
            throw CasmErrorException("Bad indirect: Jsr does not support a long indirect.",
                                     group_node.lineno, group_node.col_offset);
        }
        asm_line.addressMode = AddressModes::abs_ind_x;
    }
    else {
        asm_line.addressMode = asm_line.hasLong ? AddressModes::abs_long : AddressModes::abs;
    }
    asm_line.instruction = unique_ptr<Instruction>(new Group10);
}




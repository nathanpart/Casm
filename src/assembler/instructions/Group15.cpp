//
// Created by nathan on 7/30/20.
//

#include "Group15.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;

void Group15::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool isIndirect;
    bool hasXIndex;
    bool hasSIndex;

    node *exp_node = &group_node.child[1];
    isIndirect = isExpTreeIndirect(exp_node, hasXIndex, hasSIndex);
    expression.buildRpnList(*exp_node);

    if (!isIndirect || hasSIndex || hasXIndex) {
        throw CasmErrorException("PEI instruction only has direct page indirect (exp,x) address mode.",
                                 group_node.child[1].lineno, group_node.child[1].col_offset);
    }
    asm_line.instruction = unique_ptr<Instruction>(new Group15);
    asm_line.addressMode = AddressModes::dp_ind_x;
    asm_line.expressionList.push_back(expression);
}

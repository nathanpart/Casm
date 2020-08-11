//
// Created by nathan on 7/30/20.
//

#include "Group15.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"
#include "../Error.h"

using namespace std;

void Group15::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool isIndirect;
    bool hasXIndex;
    bool hasSIndex;

    node *exp_node = &group_node.child[1];
    isIndirect = isExpTreeIndirect(exp_node, hasXIndex, hasSIndex);
    expression.buildRpnList(*exp_node, asm_line.lineText);

    if (!isIndirect || hasSIndex || hasXIndex) {
        throw CasmErrorException("PEI instruction only has direct page indirect (exp,x) address mode.",
                                 group_node.child[1].location, asm_line.lineText);
    }
    asm_line.instruction = unique_ptr<Instruction>(new Group15);
    asm_line.addressMode = AddressModes::dp_ind_x;
    asm_line.expressionList.push_back({exp_node->location, expression});
}

void Group15::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816) {
        throw CasmErrorException("Instruction only available on a 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

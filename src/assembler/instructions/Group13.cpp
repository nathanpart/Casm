//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group13.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../Error.h"

using namespace std;


void Group13::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasXIndex = false;
    string inst;

    unique_ptr<Group13> group13_ptr = make_unique<Group13>();
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
            case x_index:
                hasXIndex = true;
                break;
            case GROUP13_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "STY") {
                    group13_ptr->isStz = false;
                }
                if (inst == "STZ") {
                    group13_ptr->isStz = true;
                }
                break;
        }
    }
    asm_line.instruction = unique_ptr<Instruction>(group13_ptr.release());
    if (asm_line.hasDpZp) {
        asm_line.addressMode = hasXIndex ? AddressModes::dp_x : AddressModes::dp;
    }
    else {
        asm_line.addressMode = hasXIndex ? AddressModes::abs_x : AddressModes::abs;
    }

}

void Group13::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType == CpuType::CPU_6502 && isStz) {
        throw CasmErrorException("6502 does not have this instruction.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

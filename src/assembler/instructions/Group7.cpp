//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group7.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../Error.h"

using namespace std;

void Group7::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {

            case exp:
                expression = {};
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;

            case GROUP7_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "MVN") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstMvn());
                }
                if (inst == "MVP") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstMvp());
                }
        }
        asm_line.addressMode = AddressModes::block;
    }
}

void Group7::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816) {
        throw CasmErrorException("Instruction requires a 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

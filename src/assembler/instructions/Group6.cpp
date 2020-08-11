//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group6.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../Error.h"

using namespace std;

void Group6::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

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

            case GROUP6_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "REP") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstRep());
                }
                if (inst == "SEP") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstSep());
                }
        }
    }

}

void Group6::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816) {
        throw CasmErrorException("Instruction requires a 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }

    Instruction::pass1(asm_line, state);
}

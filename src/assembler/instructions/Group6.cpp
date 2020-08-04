//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group6.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"

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
                            expression.buildRpnList(imm_node);
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
        asm_line.expressionList.push_back(expression);
    }

}

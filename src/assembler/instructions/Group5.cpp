//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group5.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"

using namespace std;


void Group5::createInstruction(node &group_node, Line &asm_line) {
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

            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;

            case exp:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;

            case GROUP5_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "CPX") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstCpx());
                }
                if (inst == "CPY") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstCpy());
                }
        }
    }
    if (asm_line.addressMode != AddressModes::imm) {
        asm_line.addressMode = asm_line.hasDpZp ? AddressModes::dp : AddressModes::abs;
    }
}

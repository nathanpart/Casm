//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group7.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"

using namespace std;

void Group7::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {

            case exp:
                expression.buildRpnList(child_node);
                asm_line.expressionList.push_back(expression);
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

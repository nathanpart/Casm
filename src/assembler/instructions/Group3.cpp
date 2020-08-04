//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group3.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"

using namespace std;

void Group3::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case LONG:
                asm_line.hasLong = true;
                break;
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
            case exp:
                expression.buildRpnList(child_node);
                asm_line.expressionList.push_back(expression);
                break;
            case GROUP3_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "BCC") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBcc());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BCS") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBcs());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BEQ") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBeq());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BMI") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBmi());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BNE") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBne());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BPL") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBpl());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BRA") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBra());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BRL") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBrl());
                    asm_line.addressMode = AddressModes::rel_long;
                }
                if (inst == "BVC") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBvc());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "BVS") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstBvs());
                    asm_line.addressMode = AddressModes::rel;
                }
                if (inst == "JSL") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstJsl());
                    asm_line.addressMode = AddressModes::abs_long;
                }
                if (inst == "PEA") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstPea());
                    asm_line.addressMode = AddressModes::abs;
                }
                if (inst == "PER") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstPer());
                    asm_line.addressMode = AddressModes::abs;
                }
                if (inst == "TRB") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstTrb());
                    asm_line.addressMode = asm_line.hasDpZp ? AddressModes::dp : AddressModes::abs;
                }
                if (inst == "TSB") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstTsb());
                    asm_line.addressMode = asm_line.hasDpZp ? AddressModes::dp : AddressModes::abs;
                }
        }
    }
}

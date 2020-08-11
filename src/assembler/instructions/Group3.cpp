//
// Created by nathan on 7/30/20.
//

#include <string>

#include "Group3.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"

#include "../Error.h"

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
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
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

void InstBra::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType == CpuType::CPU_6502) {
        throw CasmErrorException("6502 does not have this instruction.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

void InstBrl::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816) {
        throw CasmErrorException("Instruction requires a 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

void InstPea::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816) {
        throw CasmErrorException("Instruction requires a 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

void InstPer::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816) {
        throw CasmErrorException("Instruction requires a 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);

}

void InstTrb::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType == CpuType::CPU_6502) {
        throw CasmErrorException("6502 does not have this instruction.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

void InstTsb::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType == CpuType::CPU_6502) {
        throw CasmErrorException("6502 does not have this instruction.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);
}

void InstJsl::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816) {
        throw CasmErrorException("Instruction requires a 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    Instruction::pass1(asm_line, state);

}

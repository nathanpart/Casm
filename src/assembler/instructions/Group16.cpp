//
// Created by nathan on 7/30/20.
//

#include <regex>
#include <variant>

#include "Group16.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../Error.h"

using namespace std;

static const int brk = 0x00;
static const int cop = 0x02;

void Group16::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    smatch results;

    if (regex_match(group_node.child[0].str, results, regex("brk", regex::icase))) {
        asm_line.instruction = unique_ptr<Instruction>(new Group16(brk));
    }
    else {
        asm_line.instruction = unique_ptr<Instruction>(new Group16(cop));
    }

    const auto& exp_node = group_node.child.back();
    if (exp_node.type == exp) {
        expression.buildRpnList(exp_node, asm_line.lineText);
        asm_line.expressionList.push_back({exp_node.location, expression});
    }

    asm_line.addressMode = AddressModes::imp;
}

void Group16::getObjectCode(uint8_t *ptr, Line &asm_line, AsmState &state) {
    *ptr = opCode;
    if (!asm_line.expressionList.empty()) {
        auto the_exp = asm_line.expressionList.front();
        auto v = the_exp.exp.getValue(state);

        if (v && v->value < 256) {
            ptr++;
            *ptr = v->value;
        }
        else {
            throw CasmErrorException("BRK and COP instruction can only have a one byte expression.",
                                     asm_line.instructionLoc, asm_line.lineText);
        }
    }
}

void Group16::pass1(Line &asm_line, AsmState &state) {
    if (state.cpuType != CpuType::CPU_65C816 && opCode == cop) {
        throw CasmErrorException("COP is only available on the 65C816.",
                                 asm_line.instructionLoc, asm_line.lineText);
    }
    state.defineLabel();
    state.allocateSpace(asm_line.expressionList.empty() ? 1 : 2);

}

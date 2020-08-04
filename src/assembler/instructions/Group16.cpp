//
// Created by nathan on 7/30/20.
//

#include <regex>
#include <variant>

#include "Group16.h"
#include "../../Parser/token.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;

void Group16::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    smatch results;

    if (regex_match(group_node.child[0].str, results, regex("brk", regex::icase))) {
        asm_line.instruction = unique_ptr<Instruction>(new Group16(0x00));
    }
    else {
        asm_line.instruction = unique_ptr<Instruction>(new Group16(0x02));
    }

    const auto& exp_node = group_node.child.back();
    if (exp_node.type == exp) {
        expression.buildRpnList(exp_node);
        asm_line.expressionList.push_back(expression);
    }

    asm_line.addressMode = AddressModes::imp;
}

int Group16::getSize(Line &asm_line, AsmState &state) {
    return asm_line.expressionList.empty() ? 1 : 2;
}

void Group16::getObjectCode(uint8_t *ptr, Line &asm_line, AsmState &state) {
    *ptr = opCode;
    if (!asm_line.expressionList.empty()) {
        auto the_exp = asm_line.expressionList.front();
        auto v = the_exp.evaluate(state);
        auto *value = get_if<Value>(&v);
        if (value != nullptr && value->value < 256) {
            ptr++;
            *ptr = value->value;
        }
        else {
            throw CasmErrorException("BRK and COP instruction can only have a one byte expression.",
                                     0, 0);
        }
    }
}

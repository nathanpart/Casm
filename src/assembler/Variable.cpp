//
// Created by nathan on 8/3/20.
//

#include "Variable.h"
#include "Line.h"
#include "Error.h"

void Variable::pass1() {
    assign();
}

void Variable::pass2() {
    assign();
}

void Variable::assign() {
    auto asm_line = state.getCurrentLine();
    if (!asm_line->normalLabel()) {
        throw CasmErrorException("Cannot assign to local or variable labels.",
                                 asm_line->labelLoc, asm_line->lineText);
    }
    auto exp_opt = asm_line->expressionList.back().exp.getValue(state);
    Value value = {0, ValueType::unknown, false};
    if (exp_opt) {
        value = *exp_opt;
    }
    state.assignSymbol(asm_line->label, value);
}
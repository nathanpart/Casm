//
// Created by nathan on 8/3/20.
//

#include "Variable.h"
#include "Line.h"
#include "Error.h"

void Variable::pass1(Line &asm_line, AsmState &state) {
    if (!asm_line.normalLabel()) {
        throw CasmErrorException("Cannot assign to local or variable labels.",
                                 asm_line.labelLoc, asm_line.lineText);
    }
    state.assignSymbol(asm_line.label);
}

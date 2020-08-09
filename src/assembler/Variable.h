//
// Created by nathan on 8/3/20.
//

#ifndef CASM_VARIABLE_H
#define CASM_VARIABLE_H


#include "Instruction.h"

class Variable : public Instruction {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};


#endif //CASM_VARIABLE_H

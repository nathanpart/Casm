//
// Created by nathan on 8/3/20.
//

#ifndef CASM_VARIABLE_H
#define CASM_VARIABLE_H


#include "Instruction.h"

class Variable : public Instruction {
public:
    explicit Variable(AsmState &asm_state) : Instruction(asm_state) {}

    void pass1() override;
    void pass2() override;
    void assign();
};


#endif //CASM_VARIABLE_H

//
// Created by nathan on 8/3/20.
//

#ifndef CASM_VARIABLE_H
#define CASM_VARIABLE_H


#include "Instruction.h"

class Variable : public Instruction {
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;
};


#endif //CASM_VARIABLE_H

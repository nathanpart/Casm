//
// Created by nathan on 8/3/20.
//

#ifndef CASM_REGION_H
#define CASM_REGION_H


#include "Instruction.h"

class Region : public Instruction {
    int isEnd;

public:
    explicit Region(bool is_end = false) : isEnd(is_end) {};
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;
};


#endif //CASM_REGION_H

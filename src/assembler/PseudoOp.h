//
// Created by nathan on 8/3/20.
//

#ifndef CASM_PSEUDOOP_H
#define CASM_PSEUDOOP_H

#include "Instruction.h"


class PseudoOp : public Instruction {
    int pseudoOp;

public:
    explicit PseudoOp(int pseudo_op) : pseudoOp(pseudo_op) {};
    int getSize(Line &Line, AsmState &state) override;
    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;
    static void createInstruction(node &pseudo_node, Line &asm_line);

};


#endif //CASM_PSEUDOOP_H

//
// Created by nathan on 8/3/20.
//

#ifndef CASM_PSEUDOOP_H
#define CASM_PSEUDOOP_H

#include "Instruction.h"

enum class AlignType;

class PseudoOp : public Instruction {
    int pseudoOp = 0;
    AlignType alignType;

public:
    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;
    static void createInstruction(node &pseudo_node, Line &asm_line);

    void pass1(Line &asm_line, AsmState &state) override;
    bool processFlags(AsmState& state) const;
};


#endif //CASM_PSEUDOOP_H

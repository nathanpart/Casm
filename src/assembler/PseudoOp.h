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
    std::string condSymbol;

public:
    static void createInstruction(node &pseudo_node, Line &asm_line);

    void pass1(Line &asm_line, AsmState &state) override;
    void pass2(Line &asm_line, AsmState &state) override;
    bool processFlags(AsmState& state) const;
    bool processConditionals(Line &asm_line, AsmState &state);
};


#endif //CASM_PSEUDOOP_H

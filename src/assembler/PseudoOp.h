//
// Created by nathan on 8/3/20.
//

#ifndef CASM_PSEUDOOP_H
#define CASM_PSEUDOOP_H

#include "Instruction.h"
#include "Line.h"

enum class AlignType;

class PseudoOp : public Instruction {
    int pseudoOp = 0;
    AlignType alignType{};
    std::string condSymbol;

public:
    explicit PseudoOp(AsmState &asm_state) : Instruction(asm_state) {}

    static void createInstruction(node &pseudo_node, Line &asm_line);

    void pass1() override;
    void pass2() override;
    bool processFlags(AsmState& state) const;
    bool processConditionals(Line &asm_line, AsmState &state);
    void defineLabel() {
        auto asm_line = state.getCurrentLine();
        if (asm_line->hasLabel()) {
            auto segment = state.getCurrentSegment();
            segment->defineLabel(asm_line->label);
        }
    }
};


#endif //CASM_PSEUDOOP_H

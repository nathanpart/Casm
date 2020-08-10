//
// Created by nathan on 7/20/20.
//

#ifndef CASM_LINE_H
#define CASM_LINE_H


#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "Expression.h"
#include "Instruction.h"

enum class AddressModes { imm, abs, abs_long, dp, dp_ind, dp_ind_long, abs_x, abs_y, abs_long_x, dp_x, dp_y,
        dp_ind_x, dp_ind_y, dp_ind_long_y, sr, sr_ind_y, acc, rel, rel_long, imp, long_ind, abs_ind, abs_ind_x,
        block, unknown};

enum class LineTypes { cpu, pseudo_op, variable, segment, macro, include};

struct ExpItem {
    Location loc;
    Expression exp;
};

struct Line {
    std::string lineText;
    Location location;

    std::string label;
    Location labelLoc;

    LineTypes lineType = LineTypes::cpu;
    std::unique_ptr<Instruction> instruction;
    Location instructionLoc;
    AddressModes addressMode = AddressModes::unknown;
    int size = 0;
    bool hasWide = false;
    bool hasShort = false;
    bool hasLong = false;
    bool hasDpZp = false;

    bool hasPara = false;
    bool hasPage = false;


    std::vector<ExpItem> expressionList;


    bool fromTree(node &tree);

    // Label field type queries
    int getLength(AsmState &state) { return instruction->getSize(*this, state); }
    bool isLocalLabel() { return !label.empty() && label.back() == ':'; }
    bool isVariable() { return !label.empty() && label.back() == '&'; }
    [[nodiscard]] bool hasLabel() const { return !label.empty(); }
    bool normalLabel() { return !label.empty() && label.back() != ':' && label.back() != '&'; }
};



#endif //CASM_LINE_H

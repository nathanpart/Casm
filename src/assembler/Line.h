//
// Created by nathan on 7/20/20.
//

#ifndef CASM_LINE_H
#define CASM_LINE_H


#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "AddressMode.h"
#include "Expression.h"
#include "Instruction.h"


enum class LineTypes { cpu, pseudo_op, variable, segment, macro, include, external};

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

    std::vector<ExpItem> expressionList;
    std::vector<uint8_t> opCodes;

    bool fromTree(node &tree);

    // Label field type queries
    bool isLocalLabel() { return !label.empty() && label.back() == ':'; }
    bool isVariable() { return !label.empty() && label.back() == '&'; }
    [[nodiscard]] bool hasLabel() const { return !label.empty(); }
    bool normalLabel() { return !label.empty() && label.back() != ':' && label.back() != '&'; }
};



#endif //CASM_LINE_H

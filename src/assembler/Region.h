//
// Created by nathan on 8/3/20.
//

#ifndef CASM_REGION_H
#define CASM_REGION_H

#include <string>

#include "Instruction.h"

enum class AlignType;
enum class SegmentType;

class Region : public Instruction {
    bool isEndSegment = false;
    std::string segmentName;
    AlignType alignment{};
    SegmentType segmentType{};

public:
    explicit Region(AsmState &asm_state) : Instruction(asm_state) {}

    static void createInstruction(node &inst_node, Line &asm_line);

    void pass1() override;
    void pass2() override;
};


#endif //CASM_REGION_H

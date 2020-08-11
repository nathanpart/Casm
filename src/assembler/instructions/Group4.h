//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP4_H
#define CASM_GROUP4_H


#include "../Instruction.h"

class Group4 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);
};

class InstBit : public Group4 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

class InstLdy : public Group4 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};



#endif //CASM_GROUP4_H

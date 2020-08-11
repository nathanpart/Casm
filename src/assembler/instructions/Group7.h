//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP7_H
#define CASM_GROUP7_H


#include "../Instruction.h"

class Group7 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);

    void pass1(Line &asm_line, AsmState &state) override;
};

class InstMvn : public Group7 {

};

class InstMvp : public Group7 {

};


#endif //CASM_GROUP7_H

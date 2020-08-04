//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP5_H
#define CASM_GROUP5_H


#include "../Instruction.h"

class Group5 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);
};

class InstCpx : public Group5 {

};

class InstCpy : public Group5 {

};


#endif //CASM_GROUP5_H

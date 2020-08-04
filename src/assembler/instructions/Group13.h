//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP13_H
#define CASM_GROUP13_H


#include "../Instruction.h"

class Group13 : public Instruction {
    bool isStz = false;

public:
    static void createInstruction(node &group_node, Line &asm_line);
};

class InstSty : public Group13 {

};

class InstStz : public Group13 {

};


#endif //CASM_GROUP13_H

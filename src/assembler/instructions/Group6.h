//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP6_H
#define CASM_GROUP6_H


#include "../Instruction.h"

class Group6 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);
};

class InstRep : public Group6 {

};

class InstSep : public Group6 {

};



#endif //CASM_GROUP6_H

//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP12_H
#define CASM_GROUP12_H


#include "../Instruction.h"

class Group12 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);
};



#endif //CASM_GROUP12_H

//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP4_H
#define CASM_GROUP4_H


#include "../Instruction.h"

class Group4 : public Instruction {
public:
    static void createInstruction(node &node, Line &line);
};

class InstBit : public Group4 {

};

class InstLdy : public Group4 {

};



#endif //CASM_GROUP4_H

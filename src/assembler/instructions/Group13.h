//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP13_H
#define CASM_GROUP13_H


#include "../Instruction.h"

class Group13 : public Instruction {
public:
    static void createInstruction(node &node, Line &line);
};

class InstSty : public Group13 {

};

class InstStz : public Group13 {

};


#endif //CASM_GROUP13_H

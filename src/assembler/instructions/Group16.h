//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP16_H
#define CASM_GROUP16_H


#include "../Instruction.h"

class Group16 : public Instruction {
public:
    static void createInstruction(node &node, Line &line);
};



#endif //CASM_GROUP16_H

//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP15_H
#define CASM_GROUP15_H


#include "../Instruction.h"

class Group15 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);

    void pass1(Line &asm_line, AsmState &state) override;
};



#endif //CASM_GROUP15_H

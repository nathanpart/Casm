//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP11_H
#define CASM_GROUP11_H


#include "../Instruction.h"

class Group11 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);

    void pass1(Line &asm_line, AsmState &state) override;
};



#endif //CASM_GROUP11_H

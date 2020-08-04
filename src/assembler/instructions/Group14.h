//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP14_H
#define CASM_GROUP14_H


#include "../Instruction.h"

class Group14 : public Instruction {
    int opCode;
public:
    explicit Group14(int op_code) : opCode(op_code) {};
    static void createInstruction(node &group_node, Line &asm_line);

    int getSize(Line &Line, AsmState &state) override {
        return 1;
    };

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override {
        *ptr = opCode;
    };
};



#endif //CASM_GROUP14_H

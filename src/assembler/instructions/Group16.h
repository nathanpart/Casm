//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP16_H
#define CASM_GROUP16_H


#include "../Instruction.h"

class Group16 : public Instruction {
    int opCode;

public:
    explicit Group16(int op_code) : opCode(op_code) {}

    int getSize(Line &asm_line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &asm_line, AsmState &state) override;;
    static void createInstruction(node &group_node, Line &asm_line);
};



#endif //CASM_GROUP16_H

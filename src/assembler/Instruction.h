//
// Created by nathan on 7/27/20.
//

#ifndef CASM_INSTRUCTION_H
#define CASM_INSTRUCTION_H


#include <memory>
//#include "Line.h"
#include "AsmState.h"

class Line;
class node;
enum class AddressMode;

class Instruction {
public:
    virtual int getSize(Line &Line, AsmState &state) { return 0; }
    virtual void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {}
    virtual void pass1(Line &asm_line, AsmState &state);


    static void createInstruction(node &inst_node, Line &asm_line);
};


#endif //CASM_INSTRUCTION_H

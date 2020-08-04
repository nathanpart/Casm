//
// Created by nathan on 8/3/20.
//

#ifndef CASM_MACRO_H
#define CASM_MACRO_H

#include <string>
#include <vector>

#include "Instruction.h"

class Macro : public Instruction {
    std::string macroName;
    std::vector<std::string> argumentList;
    bool isDefinition = false;
    bool isEndMac = false;


public:
    static void createInstruction(node &macro_node, Line &asm_line);
    int getSize(Line &Line, AsmState &state) override;
    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};


#endif //CASM_MACRO_H

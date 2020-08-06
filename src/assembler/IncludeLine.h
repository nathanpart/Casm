//
// Created by nathan on 8/5/20.
//

#ifndef CASM_INCLUDELINE_H
#define CASM_INCLUDELINE_H

#include <string>

#include "Instruction.h"

class IncludeLine : Instruction {
    std::string fileName;

public:
    explicit IncludeLine(std::string &file_name) : fileName(file_name) {}

    static void createInstruction(node &inst_node, Line &asm_line);
    std::string getName() {
        return fileName;
    }
};


#endif //CASM_INCLUDELINE_H

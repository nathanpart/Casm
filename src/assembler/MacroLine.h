//
// Created by nathan on 8/3/20.
//

#ifndef CASM_MACROLINE_H
#define CASM_MACROLINE_H

#include <string>
#include <vector>

#include "Instruction.h"

class MacroLine : public Instruction {
    std::string macroName;
    std::vector<std::string> argumentList;
    bool isDefinition = false;
    bool isEndMac = false;

public:
    static void createInstruction(node &macro_node, Line &asm_line);
    [[nodiscard]] bool isDef() const {
        return isDefinition;
    }

    std::string getName() {
        return macroName;
    }

    std::vector<std::string> getArgs() {
        return argumentList;
    }

};


#endif //CASM_MACROLINE_H

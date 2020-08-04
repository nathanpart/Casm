//
// Created by nathan on 8/3/20.
//

#include "Macro.h"
#include "Line.h"
#include "../Parser/token.h"

using namespace std;

int Macro::getSize(Line &Line, AsmState &state) {
    return 0;
}

void Macro::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

void Macro::createInstruction(node &macro_node, Line &asm_line) {
    auto *macro = new Macro();

    for (const auto& mac_node: macro_node.child) {
        switch (mac_node.type) {
            case NAME:
                macro->macroName = mac_node.str;
                macro->isDefinition = false;
                break;
            case ARG:
                macro->argumentList.push_back(mac_node.str);
                break;
            case MACRO:
                macro->isDefinition = true;
                break;
            case ENDM:
                macro->isEndMac = true;
                break;
        }
    }
    asm_line.instruction = unique_ptr<Instruction>(macro);
}

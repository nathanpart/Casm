//
// Created by nathan on 8/3/20.
//

#include "MacroLine.h"
#include "Line.h"
#include "../Parser/token.h"

using namespace std;


void MacroLine::createInstruction(node &macro_node, Line &asm_line) {
    auto macro = make_shared<MacroLine>(MacroLine(*asm_line.state));

    for (const auto& mac_node: macro_node.child) {
        switch (mac_node.type) {
            case NAME:
                macro->macroName = mac_node.str;
                macro->isDefinition = false;
                break;
            case ARG:
                // Push the argument minus the ` delimiters.
                macro->argumentList.push_back(mac_node.str.substr(1, macro_node.str.length() - 2));
                break;
            case MACRO:
                macro->isDefinition = true;
                break;
            case ENDM:
                macro->isEndMac = true;
                break;
        }
    }
    asm_line.instruction = macro;
}

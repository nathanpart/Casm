//
// Created by nathan on 8/5/20.
//

#include "IncludeLine.h"
#include "Line.h"

using namespace std;

void IncludeLine::createInstruction(node &inst_node, Line &asm_line) {
    string str = inst_node.child.back().str;
    asm_line.instruction = make_shared<Instruction>(IncludeLine(str));
}

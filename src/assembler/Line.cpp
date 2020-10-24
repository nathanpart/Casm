//
// Created by nathan on 7/20/20.
//

#include "Line.h"
#include "../Parser/token.h"
#include "../Parser/graminit.h"
#include "../Parser/node.h"
#include "Instruction.h"
#include "Segment.h"
#include "Error.h"

using namespace std;

bool Line::fromTree(node &tree) {
   for (auto &node: tree.child) {
       if (ISTERMINAL(node.type)) {
           return node.type != ENDMARKER;
       }

       if (node.type == ::label) {
           label = node.child.front().str;  // Extract the NAME leaf
           if (node.child.back().type == COLON) {
               label += ':';
           }
           if (node.child.back().type == AMPER) {
               label += '&';
           }
       }
       else {
           Instruction::createInstruction(node, *this);
       }
   }
   return true;
}


void Line::pass1() {
    if (state->isActive()) {
        if (instruction == nullptr && label.empty()) {
            if (state->inSegment()) {
                auto seg = state->getCurrentSegment();
                seg->defineLabel(label);
            }
            else {
                throw CasmErrorException("Not currently in a segment.",
                                         labelLoc, lineText);
            }
        }
        instruction->pass1();
    }
    else {
        if (lineType == LineTypes::pseudo_op) {
            instruction->pass1();
        }
    }
}

void Line::pass2() {
    if (state->isActive() || lineType == LineTypes::pseudo_op) {
        instruction->pass2();
    }
}
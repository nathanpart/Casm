//
// Created by nathan on 7/20/20.
//

#include "Line.h"
#include "../Parser/token.h"
#include "../Parser/graminit.h"
#include "../Parser/node.h"
#include "Instruction.h"

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

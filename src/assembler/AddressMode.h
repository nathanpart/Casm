//
// Created by nathan on 7/22/20.
//

#ifndef CASM_ADDRESSMODE_H
#define CASM_ADDRESSMODE_H


#include "../Parser/node.h"
#include "Line.h"

bool isExpTreeIndirect(node *&exp_node, bool &hasXIndex, bool &hasSIndex);
int getAddressModeSize(AddressModes mode, AsmState &state, bool isAccum = true);

#endif //CASM_ADDRESSMODE_H

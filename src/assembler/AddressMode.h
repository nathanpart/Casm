//
// Created by nathan on 7/22/20.
//

#ifndef CASM_ADDRESSMODE_H
#define CASM_ADDRESSMODE_H


#include "../Parser/node.h"

class AsmState;
class Line;

enum class AddressModes { imm, abs, abs_long, dp, dp_ind, dp_ind_long, abs_x, abs_y, abs_long_x, dp_x, dp_y,
    dp_ind_x, dp_ind_y, dp_ind_long_y, sr, sr_ind_y, acc, rel, rel_long, imp, long_ind, abs_ind, abs_ind_x,
    abs_ind_long, block, dp_rel, unknown};


bool isExpTreeIndirect(node *&exp_node, bool &hasXIndex, bool &hasSIndex);
int getAddressModeSize(AddressModes mode, AsmState &state, char imm_operand_size = 0);
void storeImmediate(Line &asm_line, AsmState &state, char imm_size_flag);
void storeAbsolute(Line &asm_line, AsmState &state);
void storeLong(Line &asm_line, AsmState &state);
void storeByte(Line &asm_line, AsmState &state);
void storeRelative(Line &asm_line, AsmState &state);
void storeRelativeLong(Line &asm_line, AsmState &state);
void storeBlock(Line &asm_line, AsmState &state);
void storeByteAndRelative(Line &asm_line, AsmState &state);
void writeOperand(AddressModes mode, AsmState &state, char imm_operand_size);


#endif //CASM_ADDRESSMODE_H

//
// Created by nathan on 7/22/20.
//

#include "../Parser/graminit.h"
#include "AddressMode.h"
#include "../Parser/token.h"
#include "Line.h"
#include "Error.h"

using namespace std;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
static bool findAtom(node *&cur_node) {
    if (cur_node->type == atom)
        return true;
    if (cur_node->child.size() > 1) {
        return false;
    }
    cur_node = &cur_node->child.front();
    return findAtom(cur_node);
}
#pragma clang diagnostic pop


bool isExpTreeIndirect(node *&exp_node, bool &hasXIndex, bool &hasSIndex) {
    node *node_ptr = exp_node;
    if (!findAtom(node_ptr)) {
        return false;
    }
    int atom_size = node_ptr->child.size();
    if (node_ptr->child.front().type != LPAR) {
        return false;
    }
    if (atom_size == 4) {
        node &indexNode = node_ptr->child[2];
        hasXIndex = indexNode.child[1].type == XREG;
        hasSIndex = indexNode.child[1].type == SREG;
    }
    else {
        hasSIndex = false;
        hasXIndex = false;
    }
    exp_node = &node_ptr->child[1];
    return true;
}


int getAddressModeSize(AddressModes mode, AsmState &state, char imm_operand_size) {
    switch (mode) {
        case AddressModes::imm:
            if (state.cpuType != CpuType::CPU_65C816) {
                return 1;
            }
            if (state.currentLine->hasWide) {
                return 2;
            }
            if (state.currentLine->hasShort) {
                return 1;
            }
            if (imm_operand_size == 'A') {
                return state.isAccumWide ? 2 : 1;
            }
            else if (imm_operand_size == 'X'){
                return state.isIndexWide ? 2 : 1;
            }
            else
                return 1;

        case AddressModes::acc:
        case AddressModes::imp:
        case AddressModes::unknown:
            return 0;

        case AddressModes::dp:
        case AddressModes::dp_ind:
        case AddressModes::dp_ind_long:
        case AddressModes::dp_x:
        case AddressModes::dp_y:
        case AddressModes::dp_ind_x:
        case AddressModes::dp_ind_y:
        case AddressModes::dp_ind_long_y:
        case AddressModes::sr:
        case AddressModes::sr_ind_y:
        case AddressModes::rel:
            return 1;

        case AddressModes::abs:
        case AddressModes::abs_x:
        case AddressModes::abs_y:
        case AddressModes::rel_long:
        case AddressModes::long_ind:
        case AddressModes::abs_ind:
        case AddressModes::abs_ind_x:
        case AddressModes::block:
        case AddressModes::abs_ind_long:
        case AddressModes::dp_rel:
            return 2;

        case AddressModes::abs_long:
        case AddressModes::abs_long_x:
            return 3;
    }
}

void writeOperand(AddressModes mode, AsmState &state, char imm_operand_size) {
    switch (mode) {
        case AddressModes::imm:
            storeImmediate(*state.currentLine, state, imm_operand_size);
            break;
        case AddressModes::dp:
        case AddressModes::dp_ind:
        case AddressModes::dp_ind_long:
        case AddressModes::dp_x:
        case AddressModes::dp_y:
        case AddressModes::dp_ind_x:
        case AddressModes::dp_ind_y:
        case AddressModes::dp_ind_long_y:
        case AddressModes::sr:
        case AddressModes::sr_ind_y:
            storeByte(state.currentLine->expressionList.front(), *state.currentLine, state);
            break;
        case AddressModes::abs:
        case AddressModes::abs_x:
        case AddressModes::abs_y:
        case AddressModes::long_ind:
        case AddressModes::abs_ind:
        case AddressModes::abs_ind_x:
        case AddressModes::abs_ind_long:
            storeAbsolute(state.currentLine->expressionList.front(), *state.currentLine, state);
            break;
        case AddressModes::rel:
            storeRelative(state.currentLine->expressionList.front(), *state.currentLine, state);
            break;
        case AddressModes::rel_long:
            storeRelativeLong(*state.currentLine, state);
            break;
        case AddressModes::block:
            storeBlock(*state.currentLine, state);
            break;
        case AddressModes::dp_rel:
            storeByteAndRelative(*state.currentLine, state);
            break;
        case AddressModes::abs_long:
        case AddressModes::abs_long_x:
            storeLong(state.currentLine->expressionList.front(), *state.currentLine, state);
        default:
            ;               // accumulator & implied addressing modes have no operand
    }
}

void storeImmediate(Line &asm_line, AsmState &state, char imm_size_flag) {
    if (state.cpuType == CpuType::CPU_65C816 &&
            (asm_line.hasWide ||
            (imm_size_flag == 'A' && state.isAccumWide) || (imm_size_flag == 'X' && state.isIndexWide))) {
        storeAbsolute(asm_line.expressionList.front(), asm_line, state);
        return;
    }
    storeByte(asm_line.expressionList.front(), asm_line, state);
}

void storeAbsolute(ExpItem &exp_item, Line &asm_line, AsmState &state, bool isBigEndian, bool isDA) {
    unsigned exp_value;
    auto exp_opt = exp_item.exp.getValue(state);
    if (!exp_opt) {
        throw CasmErrorException("Unable to resolve expression.", exp_item.loc,
                                 asm_line.lineText);
    }
    exp_value = exp_opt->value - (isDA ? 2 : 0);
    if (exp_opt->value - (isDA ? 2 : 0) < -32768 || exp_opt->value - (isDA ? 2 : 0) > 0xFFFF) {
        // Warning value out of range
    }

    state.addRelocationEntry(*exp_opt, 2, asm_line.expressionList.front().loc);

    exp_value &= 0xFFFFu;
    if (isBigEndian) {
        state.storeByte((exp_value >> 8u) & 0xFFu);
        state.storeByte(exp_value & 0xFFu);
    }
    else {
        state.storeByte(exp_value & 0xFFu);
        state.storeByte((exp_value >> 8u) & 0xFFu);
    }
}

void storeLong(ExpItem &exp_item, Line &asm_line, AsmState &state) {
    unsigned exp_value;
    auto exp_opt = exp_item.exp.getValue(state);
    if (!exp_opt) {
        throw CasmErrorException("Unable to resolve expression.", exp_item.loc,
                                 asm_line.lineText);
    }
    exp_value = exp_opt->value;
    if (exp_opt->value < -8388608 || exp_opt->value > 0xFFFFFF) {
        // Warning value out of range
    }

    state.addRelocationEntry(*exp_opt, 3, asm_line.expressionList.front().loc);

    exp_value &= 0xFFFFFFu;
    state.storeByte(exp_value & 0xFFu);
    state.storeByte((exp_value >> 8u) & 0xFFu);
    state.storeByte((exp_value >> 16u) & 0xFFu);
}

void storeByte(ExpItem &exp_item, Line &asm_line, AsmState &state) {
    unsigned exp_value;
    auto exp_opt = exp_item.exp.getValue(state);
    if (!exp_opt) {
        throw CasmErrorException("Unable to resolve expression.", exp_item.loc,
                                 asm_line.lineText);
    }

    state.addRelocationEntry(*exp_opt, 1, asm_line.expressionList.front().loc);

    exp_value = exp_opt->value;
    if (exp_opt->value < -128 || exp_opt->value > 255) {
        // Warning value out of range
    }
    state.storeByte(exp_value & 0xFFu);
}

void storeRelative(ExpItem &exp_item, Line &asm_line, AsmState &state) {
    auto exp_opt = exp_item.exp.getValue(state);
    if (!exp_opt) {
        throw CasmErrorException("Unable to resolve expression.", exp_item.loc,
                                 asm_line.lineText);
    }
    int offset = exp_opt->value - (state.getCurrentOffset(exp_item.loc) + 1);
    if (offset < -128 || offset > 127) {
        throw CasmErrorException("Relative destination is out of range.", exp_item.loc,
                                 asm_line.lineText);
    }
    state.storeByte(offset);
}

void storeRelativeLong(Line &asm_line, AsmState &state) {
    auto exp_opt = asm_line.expressionList.front().exp.getValue(state);
    if (!exp_opt) {
        throw CasmErrorException("Unable to resolve expression.", asm_line.expressionList.front().loc,
                                 asm_line.lineText);
    }
    int offset = exp_opt->value - (state.getCurrentOffset(asm_line.expressionList.front().loc) + 1);
    if (offset < -32768 || offset > 32767) {
        throw CasmErrorException("Relative long destination is out of range.", asm_line.expressionList.front().loc,
                                 asm_line.lineText);
    }
    unsigned u_offset = offset;
    state.storeByte(u_offset & 0xFFu);
    state.storeByte((u_offset >> 8u) & 0xFFu);
}

void storeBlock(Line &asm_line, AsmState &state) {
    storeByte(asm_line.expressionList.front(), asm_line, state);
    storeByte(asm_line.expressionList.back(), asm_line, state);
}

void storeByteAndRelative(Line &asm_line, AsmState &state) {
    storeByte(asm_line.expressionList.front(), asm_line, state);
    storeRelative(asm_line.expressionList.back(), asm_line, state);
}

//
// Created by nathan on 7/22/20.
//

#include <set>

#include "../Parser/graminit.h"
#include "AddressMode.h"
#include "../Parser/token.h"
#include "Line.h"


using namespace std;

static bool findAtom(node *&cur_node) {
    if (cur_node->type == atom)
        return true;
    if (cur_node->child.size() > 1) {
        return false;
    }
    cur_node = &cur_node->child.front();
    return findAtom(cur_node);
}

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


int getAddressModeSize(AddressModes mode, AsmState &state, bool isAccum) {
    switch (mode) {
        case AddressModes::imm:
            if (state.currentLine->hasWide) {
                return 2;
            }
            if (state.currentLine->hasShort) {
                return 1;
            }
            if (isAccum) {
                return state.isAccumWide ? 2 : 1;
            }
            else {
                return state.isIndexWide ? 2 : 1;
            }

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
            return 2;

        case AddressModes::abs_long:
        case AddressModes::abs_long_x:
            return 3;
    }
}

bool hasAddressMode(AsmState &state) {
    static set<AddressModes> cpu65502Modes = {
            AddressModes::imm, AddressModes::abs, AddressModes::dp, AddressModes::abs_x, AddressModes::abs_y,
            AddressModes::dp_x, AddressModes::dp_y, AddressModes::dp_ind_x, AddressModes::dp_ind_y,
            AddressModes::acc, AddressModes::rel, AddressModes::imp, AddressModes::abs_ind
    };

    static set<AddressModes> cpu65C02Modes = {
            AddressModes::imm, AddressModes::abs, AddressModes::dp, AddressModes::abs_x, AddressModes::abs_y,
            AddressModes::dp_x, AddressModes::dp_y, AddressModes::dp_ind_x, AddressModes::dp_ind_y,
            AddressModes::acc, AddressModes::rel, AddressModes::imp, AddressModes::abs_ind, AddressModes::abs_ind_x,
            AddressModes::dp_ind
    };

    static set<AddressModes> cpu65816Modes = {
            AddressModes::imm, AddressModes::abs, AddressModes::dp, AddressModes::abs_x, AddressModes::abs_y,
            AddressModes::dp_x, AddressModes::dp_y, AddressModes::dp_ind_x, AddressModes::dp_ind_y,
            AddressModes::acc, AddressModes::rel, AddressModes::imp, AddressModes::abs_ind, AddressModes::abs_ind_x,
            AddressModes::abs_ind_long, AddressModes::abs_long, AddressModes::abs_long_x, AddressModes::dp_ind,
            AddressModes::dp_ind_long, AddressModes::dp_ind_long_y, AddressModes::rel_long, AddressModes::sr,
            AddressModes::sr_ind_y, AddressModes::block
    };

    switch (state.cpuType) {
        case CpuType::CPU_6502:
            if (cpu65502Modes.count(state.currentLine->addressMode) == 0)
                return false;
            break;
        case CpuType::CPU_65C02:
            if (cpu65C02Modes.count(state.currentLine->addressMode) == 0)
                return false;
            break;
        case CpuType::CPU_65C816:
            if (cpu65816Modes.count(state.currentLine->addressMode) == 0)
                return false;
    }
    return true;
}

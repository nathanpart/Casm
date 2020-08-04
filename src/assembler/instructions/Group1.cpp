//
// Created by nathan on 7/30/20.
//

#include "Group1.h"
#include "../../Parser/graminit.h"
#include "../../Parser/token.h"
#include "../Line.h"
#include "../AddressMode.h"

using namespace std;

void Group1::createInstruction(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasYIndex = false;
    bool hasXIndex = false;
    bool hasSIndex = false;
    bool hasXIndirect = false;
    bool hasSIndirect = false;
    bool isIndirect = false;
    bool isIndirectLong = false;
    string inst;

    for (auto& child_node: group_node.child) {
        node *work_ptr = &child_node;
        switch (child_node.type) {
            case imm_exp:
                asm_line.addressMode = AddressModes::imm;
                for (const auto &imm_node: child_node.child) {
                    switch (imm_node.type) {
                        case WIDE:
                            asm_line.hasWide = true;
                            break;
                        case SHORT:
                            asm_line.hasShort = true;
                            break;
                        case exp:
                            expression.buildRpnList(imm_node);
                            break;
                        default:;  // Don't need to do anything with the hash
                    }
                }
                break;
            case LONG:
                asm_line.hasLong = true;
                break;
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case exp:
                isIndirect = isExpTreeIndirect(work_ptr, hasXIndirect, hasSIndirect);
                expression.buildRpnList(*work_ptr);
                break;
            case xys_index:
                hasXIndex = child_node.child[1].type == XREG;
                hasYIndex = child_node.child[1].type == YREG;
                hasSIndex = child_node.child[1].type == SREG;
                break;
            case long_ind_exp:
                expression.buildRpnList(child_node.child[1]);
                hasYIndex = child_node.child.size() == 4;
                isIndirectLong = true;
                break;
            case GROUP1_INST:
                inst = child_node.str;
                for (char &ch: inst) {
                    ch = static_cast<char>(toupper(ch));
                }
                if (inst == "ADC") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstAdc());
                }
                if (inst == "AND") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstAnd());
                }
                if (inst == "CMP") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstCmp());
                }
                if (inst == "EOR") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstEor());
                }
                if (inst == "LDA") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstLda());
                }
                if (inst == "ORA") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstOra());
                }
                if (inst == "SBC") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstSbc());
                }
                if (inst == "STA") {
                    asm_line.instruction = unique_ptr<Instruction>(new InstSta());
                }

            default:
                ; // Should not get here as per grammar and the parser
        }
    }

    // Set the addressing mode
    if (asm_line.addressMode == AddressModes::unknown) {
        if (!isIndirect && !asm_line.hasLong && !asm_line.hasDpZp && !hasXIndex && !hasYIndex && !hasSIndex) {
            asm_line.addressMode = AddressModes::abs;
        }
        else if (!isIndirect && asm_line.hasLong && !asm_line.hasDpZp && !hasXIndex && !hasYIndex && !hasSIndex) {
            asm_line.addressMode = AddressModes::abs_long;
        }
        else if (!isIndirect && !asm_line.hasLong && asm_line.hasDpZp && !hasXIndex && !hasYIndex && !hasSIndex) {
            asm_line.addressMode = AddressModes::dp;
        }
        else if (isIndirect && !hasXIndirect && !hasSIndirect && !hasYIndex) {
            asm_line.addressMode = AddressModes::dp_ind;
        }
        else if (isIndirectLong && !hasYIndex) {
            asm_line.addressMode = AddressModes::dp_ind_long;
        }
        else if (!isIndirect && !asm_line.hasLong && !asm_line.hasDpZp && hasXIndex && !hasYIndex && !hasSIndex) {
            asm_line.addressMode = AddressModes::abs_x;
        }
        else if (!isIndirect && asm_line.hasLong && !asm_line.hasDpZp && hasXIndex && !hasYIndex && !hasSIndex) {
            asm_line.addressMode = AddressModes::abs_long_x;
        }
        else if (!isIndirect && !asm_line.hasLong && !asm_line.hasDpZp && !hasXIndex && hasYIndex && !hasSIndex) {
            asm_line.addressMode = AddressModes::abs_y;
        }
        else if (!isIndirect && !asm_line.hasLong && asm_line.hasDpZp && hasXIndex && !hasYIndex && !hasSIndex) {
            asm_line.addressMode = AddressModes::dp_x;
        }
        else if (isIndirect && hasXIndirect && !hasSIndirect && !hasYIndex) {
            asm_line.addressMode = AddressModes::dp_ind_x;
        }
        else if (isIndirect && !hasXIndirect && !hasSIndirect && hasYIndex) {
            asm_line.addressMode = AddressModes::dp_ind_y;
        }
        else if (isIndirectLong && hasYIndex) {
            asm_line.addressMode = AddressModes::dp_ind_long_y;
        }
        else if (!isIndirect && !asm_line.hasLong && !asm_line.hasDpZp && !hasXIndex && !hasYIndex && hasSIndex) {
            asm_line.addressMode = AddressModes::sr;
        }
        else if (isIndirect && !hasXIndirect && hasSIndirect && hasYIndex) {
            asm_line.addressMode = AddressModes::sr_ind_y;
        }
        else {
            throw CasmErrorException("Bad Address mode.", group_node.child.front().lineno,
                                     group_node.child.front().col_offset);
        }
    }
    asm_line.expressionList.push_back(expression);
}

int InstAdc::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstAdc::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

int InstAnd::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstAnd::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

int InstCmp::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstCmp::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

int InstEor::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstEor::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

int InstLda::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstLda::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

int InstOra::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstOra::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

int InstSbc::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstSbc::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

int InstSta::getSize(Line &Line, AsmState &state) {
    return 0;
}

void InstSta::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

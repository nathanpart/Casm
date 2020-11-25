//
// Created by nathan on 8/19/20.
//

#include "Instruction.h"
#include "Line.h"
#include "AsmState.h"
#include "Error.h"
#include "../Parser/graminit.h"
#include "../Parser/token.h"

using namespace std;

void Instruction::createGroup1(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasYIndex = false;
    bool hasXIndex = false;
    bool hasSIndex = false;
    bool hasXIndirect = false;
    bool hasSIndirect = false;
    bool isIndirect = false;
    bool isIndirectLong = false;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

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
                        case expr:
                            expression.buildRpnList(imm_node, asm_line.lineText);
                            asm_line.expressionList.push_back({imm_node.location, expression});
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
            case expr:
                isIndirect = isExpTreeIndirect(work_ptr, hasXIndirect, hasSIndirect);
                expression.buildRpnList(*work_ptr, asm_line.lineText);
                asm_line.expressionList.push_back({work_ptr->location, expression});
                break;
            case xys_index:
                hasXIndex = child_node.child[1].type == XREG;
                hasYIndex = child_node.child[1].type == YREG;
                hasSIndex = child_node.child[1].type == SREG;
                break;
            case long_ind_exp:
                expression.buildRpnList(child_node.child[1], asm_line.lineText);
                asm_line.expressionList.push_back({child_node.child[1].location, expression});
                hasYIndex = child_node.child.size() == 4;
                isIndirectLong = true;
                break;
            case GROUP1_INST:
                cpu_instr->setInstruction(child_node.str);
                break;

            default:
                ; // Should not get here as per grammar and the parser
        }
    }

    asm_line.instruction = cpu_instr;

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
            throw CasmErrorException("Bad Address mode.", group_node.child.front().location,
                                     asm_line.lineText);
        }
    }
}

void Instruction::createGroup2(node &group_node, Line &asm_line) {
    bool hasOperand = false;
    bool hasAreg = false;
    bool hasXreg = false;
    Expression expression;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (group_node.type) {
            case AREG:
                hasAreg = true;
                hasOperand = true;
                break;
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case expr:
                hasOperand = true;
                expression.buildRpnList(group_node, asm_line.lineText);
                asm_line.expressionList.push_back({group_node.location, expression});
                break;
            case x_index:
                hasOperand = true;
                hasXreg = true;
                break;
            case GROUP2_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }

    }

    asm_line.instruction = cpu_instr;

    if (!hasOperand || hasAreg) {
        asm_line.addressMode = AddressModes::acc;
    } else {
        if (asm_line.hasDpZp) {
            asm_line.addressMode = hasXreg ? AddressModes::dp_x : AddressModes::dp;
        }
        else {
            asm_line.addressMode = hasXreg ? AddressModes::abs_x : AddressModes::abs;
        }
    }

}


void Instruction::createGroup3(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case LONG:
                asm_line.hasLong = true;
                break;
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
            case expr:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;
            case GROUP3_INST:
                cpu_instr->setInstruction(child_node.str);
                switch (cpu_instr->instruction) {
                    case CpuMnemonics::BCC:
                    case CpuMnemonics::BCS:
                    case CpuMnemonics::BEQ:
                    case CpuMnemonics::BMI:
                    case CpuMnemonics::BNE:
                    case CpuMnemonics::BPL:
                    case CpuMnemonics::BRA:
                    case CpuMnemonics::BVC:
                    case CpuMnemonics::BVS:
                        asm_line.addressMode = AddressModes::rel;
                        break;
                    case CpuMnemonics::BRL:
                        asm_line.addressMode = AddressModes::rel_long;
                        break;
                    case CpuMnemonics::JSL:
                        asm_line.addressMode = AddressModes::abs_long;
                        break;
                    case CpuMnemonics::PEA:
                    case CpuMnemonics::PER:
                        asm_line.addressMode = AddressModes::abs;
                        break;
                    case CpuMnemonics::TRB:
                    case CpuMnemonics::TSB:
                        asm_line.addressMode = asm_line.hasDpZp ? AddressModes::dp : AddressModes::abs;
                        break;
                    default:
                        ; // Everyone else is not this group's instructions
                }
                break;
        }
    }
    asm_line.instruction = cpu_instr;
}


void Instruction::createGroup4(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;
    bool hasXIndex = false;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case imm_exp:
                for (const auto& imm_node: child_node.child) {
                    switch (imm_node.type) {
                        case WIDE:
                            asm_line.hasWide = true;
                            break;
                        case SHORT:
                            asm_line.hasShort = true;
                            break;
                        case expr:
                            expression.buildRpnList(imm_node, asm_line.lineText);
                            asm_line.expressionList.push_back({imm_node.location, expression});
                            break;
                    }
                }
                asm_line.addressMode = AddressModes::imm;
                break;

            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;

            case expr:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;

            case x_index:
                hasXIndex = true;
                break;

            case GROUP4_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }
    }
    asm_line.instruction = cpu_instr;
    if (asm_line.addressMode != AddressModes::imm) {
        if (asm_line.hasDpZp) {
            asm_line.addressMode = hasXIndex ? AddressModes::dp_x : AddressModes::dp;
        }
        else {
            asm_line.addressMode = hasXIndex ? AddressModes::abs_x : AddressModes::abs;
        }
    }
}

void Instruction::createGroup5(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case imm_exp:
                for (const auto& imm_node: child_node.child) {
                    switch (imm_node.type) {
                        case WIDE:
                            asm_line.hasWide = true;
                            break;
                        case SHORT:
                            asm_line.hasShort = true;
                            break;
                        case expr:
                            expression.buildRpnList(imm_node, asm_line.lineText);
                            asm_line.expressionList.push_back({imm_node.location, expression});
                            break;
                    }
                }
                asm_line.addressMode = AddressModes::imm;
                break;

            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;

            case expr:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;

            case GROUP5_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }
    }
    asm_line.instruction = cpu_instr;
    if (asm_line.addressMode != AddressModes::imm) {
        asm_line.addressMode = asm_line.hasDpZp ? AddressModes::dp : AddressModes::abs;
    }
}

void Instruction::createGroup6(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case imm_exp:
                for (const auto& imm_node: child_node.child) {
                    switch (imm_node.type) {
                        case WIDE:
                            asm_line.hasWide = true;
                            break;
                        case SHORT:
                            asm_line.hasShort = true;
                            break;
                        case expr:
                            expression.buildRpnList(imm_node, asm_line.lineText);
                            asm_line.expressionList.push_back({imm_node.location, expression});
                            break;
                    }
                }
                asm_line.addressMode = AddressModes::imm;
                break;

            case GROUP6_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }
    }
    asm_line.instruction = cpu_instr;
}

void Instruction::createGroup7(node &group_node, Line &asm_line) {
    Expression expression;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {

            case expr:
                expression = {};
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;

            case GROUP7_INST:
                cpu_instr->setInstruction(child_node.str);
                if (cpu_instr->instruction == CpuMnemonics::MVN || cpu_instr->instruction == CpuMnemonics::MVP) {
                    asm_line.addressMode = AddressModes::block;
                }
                else {
                    asm_line.addressMode = AddressModes::dp_rel;
                }
                break;
        }
    }
    asm_line.instruction = cpu_instr;
}


void Instruction::createGroup8(node &group_node, Line &asm_line) {
    Expression expression;
    bool isIndirect;
    bool hasXIndex;
    bool hasSIndex;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (auto& child_node: group_node.child) {
        node *exp_node = &child_node;
        switch (child_node.type) {
            case LONG:
                asm_line.hasLong = true;
                break;

            case expr:
                isIndirect = isExpTreeIndirect(exp_node, hasXIndex, hasSIndex);
                expression.buildRpnList(*exp_node, asm_line.lineText);
                asm_line.expressionList.push_back({exp_node->location, expression});
                break;

            case jmp_long_ind_exp:
                isIndirect = true;
                asm_line.hasLong = true;
                expression.buildRpnList(child_node.child[1], asm_line.lineText);
                asm_line.expressionList.push_back({child_node.child[1].location, expression});
                break;
            case GROUP8_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }
    }
    asm_line.instruction = cpu_instr;

    if (isIndirect) {
        if (asm_line.hasLong) {
            asm_line.addressMode = AddressModes::abs_ind;
        }
        else
            asm_line.addressMode = hasXIndex ? AddressModes::abs_ind_x : AddressModes::abs_ind;
    }
    else {
        asm_line.addressMode = asm_line.hasLong ? AddressModes::abs_long : AddressModes::abs;
    }
}


void Instruction::createGroup9(node &group_node, Line &asm_line) {
    createGroup8(group_node, asm_line);
}

void Instruction::createGroup10(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasXIndex = false;
    bool hasSIndex = false;
    bool isIndirect = false;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (auto& child_node: group_node.child) {
        node *exp_node = &child_node;
        switch (child_node.type) {
            case LONG:
                asm_line.hasLong = true;
                break;
            case expr:
                isIndirect = isExpTreeIndirect(exp_node, hasXIndex, hasSIndex);
                expression.buildRpnList(*exp_node, asm_line.lineText);
                if (isIndirect && (!hasXIndex || hasSIndex)) {
                    throw CasmErrorException("Bad indirect: JSR only supports Absolute Indexed Indirect (exp,x).",
                                             child_node.location, asm_line.lineText);
                }
                asm_line.expressionList.push_back({child_node.location, expression});
                break;
            case GROUP10_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }
    }
    if (isIndirect) {
        if (asm_line.hasLong) {
            throw CasmErrorException("Bad indirect: Jsr does not support a long indirect.",
                                     group_node.location, asm_line.lineText);
        }
        asm_line.addressMode = AddressModes::abs_ind_x;
    }
    else {
        asm_line.addressMode = asm_line.hasLong ? AddressModes::abs_long : AddressModes::abs;
    }
    asm_line.instruction = cpu_instr;
}

void Instruction::createGroup11(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasYIndex = false;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
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
                        case expr:
                            expression.buildRpnList(imm_node, asm_line.lineText);
                            asm_line.expressionList.push_back({imm_node.location, expression});
                            break;
                        default:;  // Don't need to do anything with the hash
                    }
                }
                break;
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case expr:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;
            case y_index:
                hasYIndex = true;
                break;
            case GROUP11_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }
    }
    asm_line.instruction = cpu_instr;
    if (asm_line.addressMode != AddressModes::imm) {
        if (asm_line.hasDpZp) {
            asm_line.addressMode = hasYIndex ? AddressModes::dp_y : AddressModes::dp;
        }
        else {
            asm_line.addressMode = hasYIndex ? AddressModes::abs_y: AddressModes::abs;
        }
    }
}

void Instruction::createGroup12(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasYIndex = false;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case expr:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;
            case y_index:
                hasYIndex = true;
                break;
            case GROUP12_INST:
                cpu_instr->setInstruction(child_node.str);
                break;

        }
    }
    asm_line.instruction = cpu_instr;
    if (asm_line.hasDpZp) {
        asm_line.addressMode = hasYIndex ? AddressModes::dp_y : AddressModes::dp;
    }
    else {
        asm_line.addressMode = hasYIndex ? AddressModes::abs_y: AddressModes::abs;
    }
}

void Instruction::createGroup13(node &group_node, Line &asm_line) {
    Expression expression;
    bool hasXIndex = false;
    string inst;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);

    for (const auto& child_node: group_node.child) {
        switch (child_node.type) {
            case DP:
            case ZP:
                asm_line.hasDpZp = true;
                break;
            case expr:
                expression.buildRpnList(child_node, asm_line.lineText);
                asm_line.expressionList.push_back({child_node.location, expression});
                break;
            case x_index:
                hasXIndex = true;
                break;
            case GROUP13_INST:
                cpu_instr->setInstruction(child_node.str);
                break;
        }
    }
    asm_line.instruction = cpu_instr;
    if (asm_line.hasDpZp) {
        asm_line.addressMode = hasXIndex ? AddressModes::dp_x : AddressModes::dp;
    }
    else {
        asm_line.addressMode = hasXIndex ? AddressModes::abs_x : AddressModes::abs;
    }

}

void Instruction::createGroup14(node &group_node, Line &asm_line) {
    auto cpu_instr = make_shared<Instruction>(*asm_line.state);
    const auto& child_node = group_node.child[0];
    smatch results;
    cpu_instr->setInstruction(child_node.str);
    asm_line.instruction = cpu_instr;
    asm_line.addressMode = AddressModes::imp;
}

void Instruction::createGroup15(node &group_node, Line &asm_line) {
    Expression expression;
    bool isIndirect;
    bool hasXIndex;
    bool hasSIndex;

    auto cpu_instr = make_shared<Instruction>(*asm_line.state);
    cpu_instr->setInstruction(group_node.str);
    node *exp_node = &group_node.child[1];
    isIndirect = isExpTreeIndirect(exp_node, hasXIndex, hasSIndex);
    expression.buildRpnList(*exp_node, asm_line.lineText);

    if (cpu_instr->instruction == CpuMnemonics::PEI) {
        if (!isIndirect || hasSIndex || !hasXIndex) {
            throw CasmErrorException("PEI instruction only has direct page indirect (exp,x) address mode.",
                                     group_node.child[1].location, asm_line.lineText);
        }
        asm_line.addressMode = AddressModes::dp_ind_x;
    }
    else {
        if (hasSIndex || hasXIndex) {
            throw CasmErrorException("RMB and SMB instructions have dp or zp addressing.",
                                     group_node.child[1].location, asm_line.lineText);
        }
        asm_line.addressMode = AddressModes::dp;
    }
    asm_line.instruction = cpu_instr;
    asm_line.expressionList.push_back({exp_node->location, expression});
}

void Instruction::createGroup16(node &group_node, Line &asm_line) {
    Expression expression;
    smatch results;
    auto cpu_instr = make_shared<Instruction>(*asm_line.state);
    cpu_instr->setInstruction(group_node.child.front().str);
    asm_line.instruction = cpu_instr;
    const auto& exp_node = group_node.child.back();
    if (exp_node.type == expr) {
        expression.buildRpnList(exp_node, asm_line.lineText);
        asm_line.expressionList.push_back({exp_node.location, expression});
        asm_line.addressMode = AddressModes::imm;
    }
    else {
        asm_line.addressMode = AddressModes::imp;
    }
}

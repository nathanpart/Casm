//
// Created by nathan on 8/3/20.
//


#include "Line.h"
#include "PseudoOp.h"
#include "../Parser/token.h"
#include "../Parser/graminit.h"
#include "Segment.h"
#include "Error.h"


using namespace std;

static void addExpr(const node &n, Line &asm_line, bool is_child = true) {
    Expression expression;
    expression.buildRpnList(is_child ? n.child.back() : n, asm_line.lineText);
    asm_line.expressionList.push_back({n.get_location(), expression});
}

void PseudoOp::createInstruction(node &pseudo_node, Line &asm_line) {
    auto op = make_shared<PseudoOp>(PseudoOp(*asm_line.state));
    switch (pseudo_node.type) {
        case CPU6502:
        case CPU65C02:
        case CPU65C816:
        case WIDEA:
        case WIDEXY:
        case SHORTA:
        case SHORTXY:
        case END_BLOCK:
        case ELSE:
        case ENDIF:
            op->pseudoOp = pseudo_node.type;
            break;
        case define_byte:
        case define_word:
            for (const auto& def_node: pseudo_node.child) {
                unique_ptr<Expression> expression;
                switch (def_node.type) {
                    case DB:
                    case DW:
                    case DBW:
                    case DA:
                        op->pseudoOp = def_node.type;
                        break;
                    case expr:
                        addExpr(def_node, asm_line, false);
                        break;
                }
            }
            break;
        case define_storage:
            op->pseudoOp = DS;
            addExpr(pseudo_node, asm_line);
            break;
        case ::align:
            op->pseudoOp = ALIGN;
            switch (pseudo_node.child.back().type) {
                case BYTE:
                    op->alignType = AlignType::byte;
                    break;
                case WORD:
                    op->alignType = AlignType::word;
                    break;
                case DWORD:
                    op->alignType = AlignType::dword;
                    break;
                case PARA:
                    op->alignType = AlignType::para;
                    break;
                case PAGE:
                    op->alignType = AlignType::page;
                    break;
            }
            break;
        case block:
            op->pseudoOp = BLOCK;
            addExpr(pseudo_node, asm_line);
            break;
        case if_def:
            op->pseudoOp = pseudo_node.child.front().type;
            for (const auto& symbol_node: pseudo_node.child.back().child) {
               switch (symbol_node.type) {
                   case NAME:
                       op->condSymbol += symbol_node.str;
                       break;
                   case COLON:
                       op->condSymbol += ':';
                       break;
                   case AMPER:
                       op->condSymbol += '&';
                       break;
                   case DOT:
                       op->condSymbol += '.';
                       break;
               }
            }
            break;
        case iff:
            op->pseudoOp = IF;
            addExpr(pseudo_node, asm_line);
            break;
    }
    asm_line.instruction = op;
}


void PseudoOp::pass1() {
    auto asm_line = state.getCurrentLine();
    int pseudo_op_size = 0;
    ExpValue value_opt;
    Location loc;
    if (processConditionals(*asm_line, state))
        return;
    if (!state.getActiveFlag())
        return;

    if (processFlags(state)) {
        defineLabel();
        return;
    }
    switch (pseudoOp) {
        case ALIGN:
            defineLabel();
            if (!state.inSegment()) {
                throw CasmErrorException("Not in a segment.", asm_line->instructionLoc, asm_line->lineText);
            }
            state.doAlignment(alignType, asm_line->instructionLoc);
            break;
        case BLOCK:
            defineLabel();
            value_opt = asm_line->expressionList.front().exp.getValue(state);
            if (!value_opt) {
                throw CasmErrorException("Block's destination address not known on pass 1.",
                                         asm_line->expressionList.front().loc, asm_line->lineText);
            }
            if (value_opt->type != ValueType::absolute || value_opt->external) {
                throw CasmErrorException("Block's target expression cannot be external or relocatable.",
                                         asm_line->expressionList.front().loc, asm_line->lineText);
            }
            if (state.inBlock()) {
                throw CasmErrorException("Blocks cannot be nested.",
                                         asm_line->instructionLoc, asm_line->lineText);
            }
            state.enterBlock(value_opt->value);
            break;
        case END_BLOCK:
            defineLabel();
            if (!state.inBlock()) {
                throw CasmErrorException("Not currently in a BLOCK.",
                                         asm_line->instructionLoc, asm_line->lineText);
            }
            state.endBlock();
            break;
        case DB:
            defineLabel();
            for (auto &exp_item: asm_line->expressionList) {
                auto string_opt = exp_item.exp.getString(state);
                pseudo_op_size += string_opt ? static_cast<int>(string_opt->length()) : 1;
            }
            state.allocateSpace(pseudo_op_size);
            break;
        case DW:
        case DBW:
        case DA:
            defineLabel();
            state.allocateSpace(static_cast<int>(asm_line->expressionList.size()) * 2);
            break;
        case DS:
            value_opt = asm_line->expressionList.front().exp.getValue(state);
            loc = asm_line->expressionList.front().loc;
            if (!value_opt) {
                throw CasmErrorException("Unable to evaluate operand for DS on pass 1.",
                                         loc, asm_line->lineText);
            }
            if (value_opt->type != ValueType::absolute || value_opt->external) {
                throw CasmErrorException("Operand for DS cannot be a relocatable.", loc, asm_line->lineText);
            }
            if (value_opt->value < 0) {
                throw CasmErrorException("operand for DS cannot be negative.", loc, asm_line->lineText);
            }
            defineLabel();
            state.allocateSpace(value_opt->value);
            break;
    }
}

void PseudoOp::pass2() {
    auto asm_line = state.getCurrentLine();
    ExpValue value_opt;
    Location loc;
    if (processConditionals(*asm_line, state))
        return;
    if (!state.getActiveFlag())
        return;
    if (processFlags(state)) {
        defineLabel();
        return;
    }
    switch (pseudoOp) {
        case ALIGN:
            if (!state.inSegment()) {
                throw CasmErrorException("Not in a segment.", asm_line->instructionLoc, asm_line->lineText);
            }
            state.doAlignment(alignType, asm_line->instructionLoc);
            break;
        case BLOCK:
            value_opt = asm_line->expressionList.front().exp.getValue(state);
            if (!value_opt) {
                throw CasmErrorException("Block's destination address not known on pass 2.",
                                         asm_line->expressionList.front().loc, asm_line->lineText);
            }
            if (value_opt->type != ValueType::absolute || value_opt->external) {
                throw CasmErrorException("Block's target expression cannot be external or relocatable.",
                                         asm_line->expressionList.front().loc, asm_line->lineText);
            }
            if (state.inBlock()) {
                throw CasmErrorException("Blocks cannot be nested.",
                                         asm_line->instructionLoc, asm_line->lineText);
            }
            state.enterBlock(value_opt->value);
            break;
        case END_BLOCK:
            if (!state.inBlock()) {
                throw CasmErrorException("Not currently in a BLOCK.",
                                         asm_line->instructionLoc, asm_line->lineText);
            }
            state.endBlock();
            break;
        case DB:
            for (auto &exp_item: asm_line->expressionList) {
                auto string_opt = exp_item.exp.getString(state);
                if (!string_opt) {
                    storeByte(exp_item, *asm_line, state);
                }
                else {
                    for(auto& ch: *string_opt) {
                        state.storeByte(ch);
                    }
                }
            }
            break;
        case DW:
            for (auto &exp_item: asm_line->expressionList) {
                storeAbsolute(exp_item, *asm_line, state);
            }
            break;
        case DBW:
            for (auto &exp_item: asm_line->expressionList) {
                storeAbsolute(exp_item, *asm_line, state, true);
            }
            break;
        case DA:
            for (auto &exp_item: asm_line->expressionList) {
                storeAbsolute(exp_item, *asm_line, state, false, true);
            }
            break;
        case DS:
            value_opt = asm_line->expressionList.front().exp.getValue(state);
            loc = asm_line->expressionList.front().loc;
            if (!value_opt) {
                throw CasmErrorException("Unable to evaluate operand for DS on pass 2.",
                                         loc, asm_line->lineText);
            }
            if (value_opt->type != ValueType::absolute || value_opt->external) {
                throw CasmErrorException("Operand for DS cannot be a relocatable.", loc, asm_line->lineText);
            }
            if (value_opt->value < 0) {
                throw CasmErrorException("operand for DS cannot be negative.", loc, asm_line->lineText);
            }
            for (int i=0; i < value_opt->value; i++) {
                state.storeByte(0);
            }
            break;
    }
}

bool PseudoOp::processFlags(AsmState &state) const {
    switch (pseudoOp) {
        case CPU6502:
            state.cpuType = CpuType::CPU_6502;
            break;
        case CPU65C02:
            state.cpuType = CpuType::CPU_65C02;
            break;
        case CPU65C816:
            state.cpuType = CpuType::CPU_65C816;
            break;
        case WIDEA:
            state.isAccumWide = true;
            break;
        case SHORTA:
            state.isAccumWide = false;
            break;
        case WIDEXY:
            state.isIndexWide = true;
            break;
        case SHORTXY:
            state.isIndexWide = false;
            break;
        default:
            return true;
    }
    return false;
}

bool PseudoOp::processConditionals(Line &asm_line, AsmState &state) {
    switch (pseudoOp) {
        case IFDEF:
            if (state.getActiveFlag())
                state.startConditionalBlock(state.hasSymbol(condSymbol));
            else
                state.startConditionalBlock(false);
            break;
        case IFNDEF:
            if (state.getActiveFlag())
                state.startConditionalBlock(!state.hasSymbol(condSymbol));
            else
                state.startConditionalBlock(false);
            break;
        case IF:
            if (state.getActiveFlag()) {
                auto value_opt = asm_line.expressionList.front().exp.getValue(state);
                state.startConditionalBlock(value_opt && !value_opt->external && value_opt->value != 0);
            }
            else
                state.startConditionalBlock(false);
            break;
        case ELSE:
            state.flipConditionalState();
            break;
        case ENDIF:
            state.endConditionalBlock();
            break;
        default:
            return true;
    }
    return false;
}

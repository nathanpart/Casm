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
    auto *op = new PseudoOp();
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
                    case exp:
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
    asm_line.instruction = unique_ptr<Instruction>(op);
}


void PseudoOp::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}


void PseudoOp::pass1(Line &asm_line, AsmState &state) {
    int pseudo_op_size = 0;
    ExpValue value_opt;
    Location loc;
    bool symbolDefined;
    if (processFlags(state)) {
        state.defineLabel();
        return;
    }
    switch (pseudoOp) {
        case ALIGN:
            state.defineLabel();
            if (!state.inSegment()) {
                throw CasmErrorException("Not in a segment.", asm_line.instructionLoc, asm_line.lineText);
            }
            state.doAlignment(alignType, asm_line.instructionLoc);
            break;
        case BLOCK:
            state.defineLabel();
            value_opt = asm_line.expressionList.front().exp.getValue(state);
            if (!value_opt) {
                throw CasmErrorException("Block's destination address not known on pass 1.",
                                         asm_line.expressionList.front().loc, asm_line.lineText);
            }
            if (value_opt->type != ValueType::absolute || value_opt->external) {
                throw CasmErrorException("Block's target expression cannot be external or relocatable.",
                                         asm_line.expressionList.front().loc, asm_line.lineText);
            }
            if (state.inBlock()) {
                throw CasmErrorException("Blocks cannot be nested.",
                                         asm_line.instructionLoc, asm_line.lineText);
            }
            state.enterBlock(value_opt->value);
            break;
        case END_BLOCK:
            state.defineLabel();
            if (!state.inBlock()) {
                throw CasmErrorException("Not currently in a BLOCK.",
                                         asm_line.instructionLoc, asm_line.lineText);
            }
            state.endBlock();
            break;
        case DB:
            state.defineLabel();
            for (auto &exp_item: asm_line.expressionList) {
                auto string_opt = exp_item.exp.getString(state);
                pseudo_op_size += string_opt ? static_cast<int>(string_opt->length()) : 1;
            }
            state.allocateSpace(pseudo_op_size);
            break;
        case DW:
        case DBW:
        case DA:
            state.defineLabel();
            state.allocateSpace(static_cast<int>(asm_line.expressionList.size()) * 2);
            break;
        case DS:
            value_opt = asm_line.expressionList.front().exp.getValue(state);
            loc = asm_line.expressionList.front().loc;
            if (!value_opt) {
                throw CasmErrorException("Unable to evaluate operand for DS on pass 1.",
                                         loc, asm_line.lineText);
            }
            if (value_opt->type != ValueType::absolute || value_opt->external) {
                throw CasmErrorException("Operand for DS cannot be a relocatable.", loc, asm_line.lineText);
            }
            if (value_opt->value < 0) {
                throw CasmErrorException("operand for DS cannot be negative.", loc, asm_line.lineText);
            }
            state.defineLabel();
            state.allocateSpace(value_opt->value);
            break;
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
                value_opt = asm_line.expressionList.front().exp.getValue(state);
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

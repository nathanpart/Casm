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
            addExpr(pseudo_node, asm_line);
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
    ExpValue value_opt;
    if (processFlags(state)) { return; }
    switch (pseudoOp) {
        case ALIGN:
            if (!state.inSegment()) {
                throw CasmErrorException("Not in a segment.", asm_line.instructionLoc, asm_line.lineText);
            }
            state.doAlignment(alignType, asm_line.instructionLoc);
            break;
        case BLOCK:
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
            if (!state.inBlock()) {
                throw CasmErrorException("Not currently in a BLOCK.",
                                         asm_line.instructionLoc, asm_line.lineText);
            }
            state.endBlock();
            break;
    }
    Instruction::pass1(asm_line, state);
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

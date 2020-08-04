//
// Created by nathan on 8/3/20.
//

#include "Line.h"
#include "PseudoOp.h"
#include "../Parser/token.h"
#include "../Parser/graminit.h"

using namespace std;

static void addExpr(node &n, Line &asm_line) {
    Expression expression;
    expression.buildRpnList(n.child.back());
    asm_line.expressionList.push_back(expression);
}

void PseudoOp::createInstruction(node &pseudo_node, Line &asm_line) {
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
            asm_line.instruction = unique_ptr<Instruction>(new PseudoOp(pseudo_node.type));
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
                        asm_line.instruction = unique_ptr<Instruction>(new PseudoOp(def_node.type));
                        break;
                    case exp:
                        expression = make_unique<Expression>();
                        expression->buildRpnList(def_node);
                        asm_line.expressionList.push_back(*expression);
                        break;
                }
            }
            break;
        case define_storage:
            asm_line.instruction = unique_ptr<Instruction>(new PseudoOp(DS));
            addExpr(pseudo_node, asm_line);
            break;
        case ::align:
            asm_line.instruction = unique_ptr<Instruction>(new PseudoOp(ALIGN));
            asm_line.hasPage = pseudo_node.child.back().type == PAGE;
            asm_line.hasPara = pseudo_node.child.back().type == PARA;
            break;
        case block:
            asm_line.instruction = unique_ptr<Instruction>(new PseudoOp(BLOCK));
            addExpr(pseudo_node, asm_line);
            break;
        case if_def:
            asm_line.instruction = unique_ptr<Instruction>(new PseudoOp(pseudo_node.child.front().type));
            addExpr(pseudo_node, asm_line);
            break;
        case iff:
            asm_line.instruction = unique_ptr<Instruction>(new PseudoOp(IF));
            addExpr(pseudo_node, asm_line);
            break;
    }
}



int PseudoOp::getSize(Line &Line, AsmState &state) {
    return 0;
}

void PseudoOp::getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) {

}

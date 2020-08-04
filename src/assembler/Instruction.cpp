//
// Created by nathan on 7/27/20.
//


#include <instructions/Group1.h>
#include <instructions/Group2.h>
#include <instructions/Group4.h>
#include <instructions/Group3.h>
#include <instructions/Group5.h>
#include <instructions/Group6.h>
#include <instructions/Group7.h>
#include <instructions/Group8.h>
#include <instructions/Group9.h>
#include <instructions/Group10.h>
#include <instructions/Group11.h>
#include <instructions/Group12.h>
#include <instructions/Group13.h>
#include <instructions/Group14.h>
#include <instructions/Group15.h>
#include <instructions/Group16.h>
#include "../Parser/token.h"
#include "../Parser/graminit.h"
#include "Instruction.h"
#include "PseudoOp.h"
#include "Line.h"
#include "Variable.h"
#include "Region.h"
#include "Macro.h"

using namespace std;

void Instruction::createInstruction(node &inst_node, Line &curLine) {
    node &child = inst_node.child.front();
    Expression expression;
    switch (inst_node.type) {
        case op_code:
            switch (child.type) {
                case GROUP1_INST:
                    Group1::createInstruction(child, curLine);
                    break;
                case GROUP2_INST:
                    Group2::createInstruction(child, curLine);
                    break;
                case GROUP3_INST:
                    Group3::createInstruction(child, curLine);
                    break;
                case GROUP4_INST:
                    Group4::createInstruction(child, curLine);
                    break;
                case GROUP5_INST:
                    Group5::createInstruction(child, curLine);
                    break;
                case GROUP6_INST:
                    Group6::createInstruction(child, curLine);
                    break;
                case GROUP7_INST:
                    Group7::createInstruction(child, curLine);
                    break;
                case GROUP8_INST:
                    Group8::createInstruction(child, curLine);
                    break;
                case GROUP9_INST:
                    Group9::createInstruction(child, curLine);
                    break;
                case GROUP10_INST:
                    Group10::createInstruction(child, curLine);
                    break;
                case GROUP11_INST:
                    Group11::createInstruction(child, curLine);
                    break;
                case GROUP12_INST:
                    Group12::createInstruction(child, curLine);
                    break;
                case GROUP13_INST:
                    Group13::createInstruction(child, curLine);
                    break;
                case GROUP14_INST:
                    Group14::createInstruction(child, curLine);
                    break;
                case GROUP15_INST:
                    Group15::createInstruction(child, curLine);
                    break;
                case GROUP16_INST:
                    Group16::createInstruction(child, curLine);
                    break;
            }
            break;

        case pseudo_op:
            PseudoOp::createInstruction(inst_node, curLine);
            break;

        case variable:
            curLine.instruction = unique_ptr<Instruction>(new Variable);
            expression.buildRpnList(inst_node.child.back());
            curLine.expressionList.push_back(expression);
            break;

        case region:
            for (const auto &region_node: inst_node.child) {
                switch (region_node.type) {
                    case PARA:
                        curLine.hasPara = true;
                        break;
                    case PAGE:
                        curLine.hasPage = true;
                        break;
                    case exp:
                        expression.buildRpnList(region_node);
                        curLine.expressionList.push_back(expression);
                        break;
                    case SEGMENT:
                        curLine.instruction = unique_ptr<Instruction>(new Region(false));
                        break;
                    case ENDS:
                        curLine.instruction = unique_ptr<Instruction>(new Region(true));
                        break;
                }
            }
            break;

        case macro:
            Macro::createInstruction(inst_node, curLine);
            break;
    }

}

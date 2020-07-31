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

void Instruction::createInstruction(node &inst_node, Line &curLine) {
    node &child = inst_node.child.front();
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

    }

}

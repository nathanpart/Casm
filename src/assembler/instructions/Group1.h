//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP1_H
#define CASM_GROUP1_H


#include "../Instruction.h"

class Group1 : public Instruction {
public:
    static void createInstruction(node &node, Line &line);
};

class InstAdc : public Group1 {

};

class InstAnd : public Group1 {

};

class InstCmp : public Group1 {

};

class InstEor : public Group1 {

};

class InstLda : public Group1 {

};

class InstOra : public Group1 {

};

class InstSbc : public Group1 {

};

class InstSta : public Group1 {

};


#endif //CASM_GROUP1_H

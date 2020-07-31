//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP2_H
#define CASM_GROUP2_H


#include "../Instruction.h"

class Group2 : public Instruction {
public:
    static void createInstruction(node &node, Line &line);
};

class InstAsl : public Group2 {

};

class InstDec : public Group2 {

};

class InstInc : public Group2 {

};

class InstLsr : public Group2 {

};

class InstRol : public Group2 {

};

class InstRor : public Group2 {

};

#endif //CASM_GROUP2_H

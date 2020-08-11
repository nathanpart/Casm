//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP1_H
#define CASM_GROUP1_H


#include "../Instruction.h"

class Group1 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);

    void pass1(Line &asm_line, AsmState &state) override;
};

class InstAdc : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;
};

class InstAnd : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};

class InstCmp : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};

class InstEor : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};

class InstLda : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};

class InstOra : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};

class InstSbc : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};

class InstSta : public Group1 {
public:
    int getSize(Line &Line, AsmState &state) override;

    void getObjectCode(uint8_t *ptr, Line &Line, AsmState &state) override;

};


#endif //CASM_GROUP1_H

//
// Created by nathan on 7/30/20.
//

#ifndef CASM_GROUP3_H
#define CASM_GROUP3_H


#include "../Instruction.h"

class Group3 : public Instruction {
public:
    static void createInstruction(node &group_node, Line &asm_line);
};

class InstBcc : public Group3 {

};

class InstBcs : public Group3 {

};

class InstBeq : public Group3 {

};

class InstBmi : public Group3 {

};

class InstBne : public Group3 {

};

class InstBpl : public Group3 {

};

class InstBra : public Group3 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

class InstBrl : public Group3 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

class InstBvc : public Group3 {

};

class InstBvs : public Group3 {

};

class InstJsl : public Group3 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

class InstPea : public Group3 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

class InstPer : public Group3 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

class InstTrb : public Group3 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

class InstTsb : public Group3 {
public:
    void pass1(Line &asm_line, AsmState &state) override;
};

#endif //CASM_GROUP3_H

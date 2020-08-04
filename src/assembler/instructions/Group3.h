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

};

class InstBrl : public Group3 {

};

class InstBvc : public Group3 {

};

class InstBvs : public Group3 {

};

class InstJsl : public Group3 {

};

class InstPea : public Group3 {

};

class InstPer : public Group3 {

};

class InstTrb : public Group3 {

};

class InstTsb : public Group3 {

};

#endif //CASM_GROUP3_H

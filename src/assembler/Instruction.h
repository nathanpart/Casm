//
// Created by nathan on 7/27/20.
//

#ifndef CASM_INSTRUCTION_H
#define CASM_INSTRUCTION_H


#include <memory>
#include <set>

#include "Cpu.h"

#include "AsmState.h"

class Line;
class node;

class Instruction {

protected:
    AsmState &state;
    CpuOpcodes *opCodes{};
    std::set<CpuType> cpus;
    char immType = 0;
    CpuMnemonics instruction{};
    uint8_t opCode{};

    bool isImport = false;
    std::string name1;
    std::string name2;
    bool hasDot = false;
    bool isProcessed = false;
public:
    explicit Instruction(AsmState &asm_state) : state(asm_state) {}

    static void createInstruction(node &inst_node, Line &asm_line);
    static void createGroup1(node &group_node, Line &asm_line);
    static void createGroup2(node &group_node, Line &asm_line);
    static void createGroup3(node &group_node, Line &asm_line);
    static void createGroup4(node &group_node, Line &asm_line);
    static void createGroup5(node &group_node, Line &asm_line);
    static void createGroup6(node &group_node, Line &asm_line);
    static void createGroup7(node &group_node, Line &asm_line);
    static void createGroup8(node &group_node, Line &asm_line);
    static void createGroup9(node &group_node, Line &asm_line);
    static void createGroup10(node &group_node, Line &asm_line);
    static void createGroup11(node &group_node, Line &asm_line);
    static void createGroup12(node &group_node, Line &asm_line);
    static void createGroup13(node &group_node, Line &asm_line);
    static void createGroup14(node &group_node, Line &asm_line);
    static void createGroup15(node &group_node, Line &asm_line);
    static void createGroup16(node &group_node, Line &asm_line);

    void setInstruction(const std::string &inst_str);
    virtual void pass1();
    virtual void pass2();
};


#endif //CASM_INSTRUCTION_H

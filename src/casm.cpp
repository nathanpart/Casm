//
// Created by nathan on 7/20/20.
//

#include "assembler/Assembler.h"

int main() {
    Assembler assembler;
    assembler.assemble("monopoly.casm");
    assembler.writeObjectFile("monopoly.cobj");
    return 0;
}
//
// Created by nathan on 7/27/20.
//

#ifndef CASM_ASMSTATE_H
#define CASM_ASMSTATE_H

#include <string>
#include <map>
#include "Assembler.h"

#include "Segment.h"
#include "Symbols.h"

//class Segment;
struct Value;
struct Line;
//struct Symbol;
//struct SymbolXref;

class AsmState {
    Assembler &assembler;

    Segment *currentSegment = nullptr;

    std::map<std::string, Segment> segments;
    std::map<std::string, Symbol> globals;
    std::map<std::string, SymbolXref> xrefs;

public:
    CpuType cpuType = CpuType::CPU_6502;
    bool isIndexWide = false;
    bool isAccumWide = false;
    bool isPassTwo = false;
    Line *currentLine = nullptr;

    explicit AsmState(Assembler &assemble) : assembler(assemble) {}
    bool resolveSymbol(const std::string& symbol_name, Value &value, int line, int col);
    int getCurrentLocation(int line, int col);
};


#endif //CASM_ASMSTATE_H

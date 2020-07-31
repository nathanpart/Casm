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

using namespace std;

//class Segment;
struct Value;
//struct Symbol;
//struct SymbolXref;

class AsmState {
    Assembler &assembler;

    Segment *currentSegment = nullptr;

    map<std::string, Segment> segments;
    map<std::string, Symbol> globals;
    map<std::string, SymbolXref> xrefs;

public:
    CpuType cpuType = CpuType::CPU_6502;
    bool isLong = false;
    bool isDirectPage = false;
    bool isWide = false;
    bool isShort = false;
    bool isIndexWide = false;
    bool isAccumWide = false;
    bool isPassTwo = false;

    explicit AsmState(Assembler &assemble) : assembler(assemble) {}
    bool resolveSymbol(const string& symbol_name, Value &value, int line, int col);
    int getCurrentLocation(int line, int col);
};


#endif //CASM_ASMSTATE_H

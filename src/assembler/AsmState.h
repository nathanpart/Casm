//
// Created by nathan on 7/27/20.
//

#ifndef CASM_ASMSTATE_H
#define CASM_ASMSTATE_H

#include <string>
#include <map>

#include "Segment.h"
#include "Symbols.h"

struct Value;
struct Line;

enum class CpuType {CPU_6502, CPU_65C02, CPU_65C816};

class AsmState {

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

    bool resolveSymbol(const std::string& symbol_name, Value &value, int line, int col);
    int getCurrentLocation(int line, int col);
};


#endif //CASM_ASMSTATE_H

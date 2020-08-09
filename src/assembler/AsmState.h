//
// Created by nathan on 7/27/20.
//

#ifndef CASM_ASMSTATE_H
#define CASM_ASMSTATE_H

#include <string>
#include <map>

#include "Location.h"
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

    bool resolveSymbol(const std::string& symbol_name, Value &value, Location &loc);
    int getCurrentOffset(Location &loc);
    void pass1(Line &line);
    bool inSegment() {
        return currentSegment != nullptr;
    }
    void endSegment() {
        currentSegment->endSegment();
        currentSegment = nullptr;
    }
    void enterSegment(std::string &name, SegmentType &seg_type, AlignType &align_type);
    void assignSymbol(std::string &name);
    void doAlignment(AlignType &align_type, Location &loc);
    void enterBlock(int block_address) { currentSegment->enterBlock(block_address); }
    bool inBlock() { inSegment() && currentSegment->inBlock(); }
    void endBlock();
};


#endif //CASM_ASMSTATE_H

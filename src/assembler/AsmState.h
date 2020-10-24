//
// Created by nathan on 7/27/20.
//

#ifndef CASM_ASMSTATE_H
#define CASM_ASMSTATE_H

#include <string>
#include <map>
#include "Cpu.h"

#include "Location.h"
#include "Segment.h"
#include "Symbols.h"


struct Value;
struct Line;

struct CondItem {
    bool isActive;
    int lineNumber;
    bool condition;
    unsigned long level;
    bool hasElse;
};


class AsmState {

    Segment *currentSegment = nullptr;

    std::map<std::string, Segment> segments;
    std::map<std::string, Symbol> globals;
    std::map<std::string, SymbolXref> xrefs;

    int globalLine = 0;
    std::vector<CondItem> conditions;
    std::map<int, CondItem> condition_starts;
    bool activeFlag = true;
    Line *& currentLine;

public:
    CpuType cpuType = CpuType::CPU_6502;
    bool isIndexWide = false;
    bool isAccumWide = false;
    bool isPassTwo = false;

    explicit AsmState(Line*& current_line) : currentLine(current_line) {}
    bool resolveSymbol(const std::string& symbol_name, Value &value, Location &loc);
    int getCurrentOffset(Location &loc);
    void pass2Setup();
    bool inSegment() {
        return currentSegment != nullptr;
    }
    void endSegment() {
        currentSegment->endSegment();
        currentSegment = nullptr;
    }
    [[nodiscard]] bool isActive() const { return activeFlag; }
    [[nodiscard]] bool inSegment() const { return currentSegment != nullptr; }
    Segment * getCurrentSegment();
    void enterSegment(std::string &name, SegmentType &seg_type, AlignType &align_type);
    void assignSymbol(std::string &name, Value &value);
    void doAlignment(AlignType &align_type, Location &loc);
    void enterBlock(int block_address) { currentSegment->enterBlock(block_address); }
    bool inBlock() { return inSegment() && currentSegment->inBlock(); }
    void endBlock();
    void allocateSpace(int size);
    void startConditionalBlock(bool state);
    void flipConditionalState();
    void endConditionalBlock();
    [[nodiscard]] bool getActiveFlag() const { return activeFlag; }
    bool hasSymbol(const std::string& symbol_name);
    void storeByte(uint8_t byt);
    void addRelocationEntry(const Value& value, int operand_size, const Location& loc);
    std::map<std::string, Segment> & getSegments() { return segments; }
    void importSymbol(const std::string& local_name, std::string& symbol_name, std::string& seg_name);
    void exportSymbol(const std::string& label_name, const std::string& extern_name);
    void incrGlobalLineNumber() { globalLine++; }
    [[maybe_unused]] std::string getCurrentSegName();
    Line * getCurrentLine() { return currentLine; }

};


#endif //CASM_ASMSTATE_H

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

class AsmStateInterface {
public:
    virtual bool resolveSymbol(const std::string& symbol_name, Value &value, Location &loc) = 0;
    virtual int getCurrentOffset(Location &loc) = 0;
    virtual void pass2Setup() = 0;
    virtual bool inSegment() = 0;
    virtual void endSegment() = 0;
    [[nodiscard]] virtual bool isActive() const = 0;
    [[nodiscard]] virtual bool inSegment() const = 0;
    virtual Segment * getCurrentSegment() = 0;
    virtual void enterSegment(std::string &name, SegmentType &seg_type, AlignType &align_type) = 0;
    virtual void assignSymbol(std::string &name, Value &value) = 0;
    virtual void doAlignment(AlignType &align_type, Location &loc) = 0;
    virtual void enterBlock(int block_address) = 0;
    virtual bool inBlock() = 0;
    virtual void endBlock() = 0;
    virtual void allocateSpace(int size) = 0;
    virtual void startConditionalBlock(bool state) = 0;
    virtual void flipConditionalState() = 0;
    virtual void endConditionalBlock() = 0;
    [[nodiscard]] virtual bool getActiveFlag() const  = 0;
    virtual bool hasSymbol(const std::string& symbol_name) = 0;
    virtual void storeByte(uint8_t byt) = 0;
    virtual void addRelocationEntry(const Value& value, int operand_size, const Location& loc) = 0;
    virtual std::map<std::string, Segment> & getSegments() = 0;
    virtual void importSymbol(const std::string& local_name, std::string& symbol_name, std::string& seg_name) = 0;
    virtual void exportSymbol(const std::string& label_name, const std::string& extern_name) = 0;
    virtual void incrGlobalLineNumber() = 0;
    [[maybe_unused]] virtual std::string getCurrentSegName() = 0;
    virtual Line * getCurrentLine() = 0;
    virtual ~AsmStateInterface() = default;
};

class AsmState : public AsmStateInterface {

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
    bool resolveSymbol(const std::string& symbol_name, Value &value, Location &loc) override;
    int getCurrentOffset(Location &loc) override;
    void pass2Setup() override;
    bool inSegment() override {
        return currentSegment != nullptr;
    }
    void endSegment() override {
        currentSegment->endSegment();
        currentSegment = nullptr;
    }
    [[nodiscard]] bool isActive() const override { return activeFlag; }
    [[nodiscard]] bool inSegment() const override { return currentSegment != nullptr; }
    Segment * getCurrentSegment() override;
    void enterSegment(std::string &name, SegmentType &seg_type, AlignType &align_type) override;
    void assignSymbol(std::string &name, Value &value) override;
    void doAlignment(AlignType &align_type, Location &loc) override;
    void enterBlock(int block_address) override { currentSegment->enterBlock(block_address); }
    bool inBlock() override { return inSegment() && currentSegment->inBlock(); }
    void endBlock() override;
    void allocateSpace(int size) override;
    void startConditionalBlock(bool state) override;
    void flipConditionalState() override;
    void endConditionalBlock() override;
    [[nodiscard]] bool getActiveFlag() const override { return activeFlag; }
    bool hasSymbol(const std::string& symbol_name) override;
    void storeByte(uint8_t byt) override;
    void addRelocationEntry(const Value& value, int operand_size, const Location& loc) override;
    std::map<std::string, Segment> & getSegments()  override { return segments; }
    void importSymbol(const std::string& local_name, std::string& symbol_name, std::string& seg_name) override;
    void exportSymbol(const std::string& label_name, const std::string& extern_name) override;
    void incrGlobalLineNumber()  override { globalLine++; }
    [[maybe_unused]] std::string getCurrentSegName() override;
    Line * getCurrentLine()  override { return currentLine; }

};


#endif //CASM_ASMSTATE_H

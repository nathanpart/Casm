//
// Created by nathan on 7/22/20.
//

#ifndef CASM_SEGMENT_H
#define CASM_SEGMENT_H

#include <string>
#include <map>
#include <utility>
#include <memory>

#include "Expression.h"
#include "Label.h"
#include "Symbols.h"
#include "ExportImport.h"
#include "ObjectFile.h"


enum class SegmentType {
    absolute = 0, zero_page = 1, direct_page = 2, normal = 4, storage = 5
};

enum class AlignType {
    byte = 0, word = 1, dword = 2, para = 3, page = 4
};

class AsmState;

enum class RelocationTypes { high_byte, low_byte, address};
struct RelocationItem {
    int offset;
    int baseOffset;
    RelocationTypes type;
};

struct SegmentArea {
    int startAddress;
    AlignType alignment;
    std::vector<uint8_t> objectCode;
    int size;
    bool isBlock;
    std::vector<RelocationItem> relocationTable;
    std::vector<int> segmentRefs;
};
using SectionList = std::vector<SegmentArea>;


class Segment {
    AsmState *state;
    std::string name;
    int baseAddress;
    SegmentType type;
    AlignType alignType;
    bool isAbsolute;

    Labels labelTable;
    std::map<std::string, Symbol> symbols;
    std::map<std::string, Symbol> variables;
    std::map<std::string, ExportItem> exports;
    std::map<std::string, ImportDeclaration> imports;
    std::vector<ImportItem> importRefs;

    SectionList sections;
    SectionList::iterator currentSection = sections.end();
    int currentOffset;

    bool isPass2 = false;
public:
    Segment() : state(nullptr), baseAddress(0), type(SegmentType::normal), alignType(AlignType::para),
        isAbsolute(true), currentOffset(0) {}
    explicit Segment(AsmState *asm_state, std::string seg_name = "", int base_address = 0,
                     SegmentType seg_type = SegmentType::normal, AlignType align_type = AlignType::byte) :
            state(asm_state), name(std::move(seg_name)), baseAddress(base_address), type(seg_type),
            alignType(align_type), isAbsolute(seg_type == SegmentType::absolute), currentOffset(0) {}

    bool resolveSymbol(std::string symbol_name, Value &value);
    void exportSymbol(const std::string& symbol_name, int offset);
    void importSymbol(const std::string& local_name, std::string symbol_name, std::string seg_name,
                      Location &loc);
    bool hasSymbol(const std::string& sym_name);
    bool hasLabel(const std::string& sym_name) { return labelTable.hasLabel(sym_name); }
    [[nodiscard]] int getOffset() const {
        if (currentSection->isBlock) {
            return currentOffset + currentSection->startAddress;
        }
        return isAbsolute ? currentOffset + baseAddress : currentOffset;
    }
    void enterSection(AlignType section_alignment, const Location &loc, const std::string &line);
    void endSegment();
    void assignSymbol(std::string &symbol_name, Value &value);
    void enterBlock(int block_address);
    bool inBlock() { return currentSection->isBlock; }
    void defineLabel(std::string &label_name);
    void allocateSpace(int size);
    void storeByte(uint8_t byt) { currentSection->objectCode.push_back(byt); }
    void pass2Setup();
    void addRelocationEntry(const Value& value, int operand_size, const Location& loc, const std::string& line_text);
    SegmentHeader getSegmentHeader(StringTable &string_table);
    SectionList& getSections() { return sections; }
    std::map<std::string, ExportItem>& getExports() { return exports; }
    std::vector<ImportItem>& getImportRefs() { return importRefs; }
    [[nodiscard]] const std::string &getName() const {
        return name;
    }
};


#endif //CASM_SEGMENT_H

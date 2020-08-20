//
// Created by nathan on 7/22/20.
//

#ifndef CASM_SEGMENT_H
#define CASM_SEGMENT_H

#include <string>
#include <map>
#include <utility>

#include "Expression.h"
#include "Label.h"
#include "Symbols.h"
#include "ExportImport.h"


enum class SegmentType {
    absolute, zero_page, direct_page, normal, storage
};

enum class AlignType {
    byte, word, dword, para, page
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


class Segment {
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

    std::vector<SegmentArea> section;
    std::vector<SegmentArea>::iterator currentSection = section.end();
    int currentOffset;

    bool isPass2 = false;
public:
    explicit Segment(std::string seg_name = "", int base_address = 0, SegmentType seg_type = SegmentType::normal,
            AlignType align_type = AlignType::byte) :
            name(std::move(seg_name)), baseAddress(base_address), type(seg_type), alignType(align_type),
            isAbsolute(seg_type == SegmentType::absolute), currentOffset(0){}

    bool resolveSymbol(std::string symbol_name, Value &value, AsmState &state);
    void exportSymbol(const std::string& symbol_name, int offset);
    void importSymbol(const std::string& local_name, std::string symbol_name, std::string seg_name,
                      Location &loc, AsmState &state);
    bool hasSymbol(const std::string& sym_name);
    [[nodiscard]] int getOffset() const {
        if (currentSection->isBlock) {
            return currentOffset + currentSection->startAddress;
        }
        return isAbsolute ? currentOffset + baseAddress : currentOffset;
    }
    void enterSection(AlignType section_alignment, const Location &loc, const std::string &line);
    void endSegment(AsmState &state);
    void assignSymbol(std::string &symbol_name, AsmState &state, Value &value);
    void enterBlock(int block_address);
    bool inBlock() { return currentSection->isBlock; }
    void defineLabel(std::string &label_name, AsmState &state);
    void allocateSpace(int size, AsmState &state);
    void storeByte(uint8_t byt) { currentSection->objectCode.push_back(byt); }
    void pass2Setup();
    void addRelocationEntry(const Value& value, int operand_size, const Location& loc, const std::string& line_text);
};


#endif //CASM_SEGMENT_H

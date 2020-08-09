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

struct RelocataItem {
    int location;
    ValueType type;
};


enum class SegmentType {
    absolute, zero_page, direct_page, normal, storage
};

enum class AlignType {
    byte, word, dword, para, page
};

class AsmState;


struct SegmentArea {
    int startAddress;
    AlignType alignment;
    uint8_t *objectCode;
    int size;
    bool isBlock;
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
    std::map<std::string, ImportItem> importRefs;

    std::vector<SegmentArea> section;
    SegmentArea *currentSection = nullptr;
    int currentOffset;

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
        return currentOffset;
    }
    void enterSection(AlignType section_alignment, const Location &loc, const std::string &line);
    void endSegment() { currentSection->size = currentOffset - currentSection->startAddress; }
    void assignSymbol(std::string &symbol_name, AsmState &state);
    void enterBlock(int block_address);
    bool inBlock() { return currentSection->isBlock; }
};


#endif //CASM_SEGMENT_H

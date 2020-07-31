//
// Created by nathan on 7/22/20.
//

#ifndef CASM_SEGMENT_H
#define CASM_SEGMENT_H

#include <string>
#include <map>

#include "Expression.h"
#include "Label.h"
#include "Symbols.h"
#include "ExportImport.h"

struct RelocataItem {
    int location;
    ValueType type;
};


enum class SegmentType {
    direct_page, normal
};

enum class AlignType {
    none, para, page
};

class AsmState;

class Segment {
    std::string name;
    int startAddress;
    SegmentType type;
    AlignType alignType;

    int baseLocation;
    Labels labelTable;
    std::map<std::string, Symbol> symbols;
    std::map<std::string, Symbol> variables;
    std::map<std::string, ExportItem> exports;
    std::map<std::string, ImportDeclaration> imports;
    std::map<std::string, ImportItem> importRefs;

    int location;
    bool inBlock;
    int blockLocation;

public:
    bool resolveSymbol(std::string symbol_name, Value &value, AsmState &state);
    void exportSymbol(const std::string& symbol_name, int offset);
    void importSymbol(const std::string& local_name, std::string symbol_name, std::string seg_name,
            int line, int col);
    bool hasSymbol(const std::string& sym_name);
    int getLocation() {
        return inBlock ? blockLocation : location;
    }
};


#endif //CASM_SEGMENT_H

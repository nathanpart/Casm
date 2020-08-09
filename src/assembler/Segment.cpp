//
// Created by nathan on 7/22/20.
//

#include <utility>
#include <string>

#include "Segment.h"
#include "Assembler.h"
#include "AsmState.h"
#include "Line.h"
#include "Error.h"

using namespace std;

bool Segment::resolveSymbol(std::string symbol_name, Value &value, AsmState &state) {
    if (symbol_name.back() == ':') {
        if (labelTable.findLocal(name, getOffset(), value.value)) {
            value.type = ValueType::relocatable;
            value.external = false;
            return true;
        }
        else {
            return false;
        }
    }
    if (symbol_name.back() == '&') {
        if (variables.count(symbol_name) == 1) {
            value = variables[symbol_name].value;
            return true;
        }
        else {
            return false;
        }
    }
    if (labelTable.findLabel(symbol_name, value.value)) {
        value.type = ValueType::relocatable;
        value.external = false;
        return true;
    }
    else if (symbols.count(symbol_name) == 1) {
        value = symbols[symbol_name].value;
        return true;
    }
    else  if (imports.count(symbol_name) == 1){
        auto import = imports[symbol_name];
        value.external = true;
        value.externalName = import.name;
        value.externalSeg = import.segmentName;
        value.value = 0;
        value.type = state.currentLine->hasLong ? ValueType::big : ValueType::relocatable;
        return true;
    }

    return false;
}

void Segment::exportSymbol(const std::string& symbol_name, int offset) {
    if (exports.count(symbol_name) == 0) {
        ExportItem item = {symbol_name, name, offset};
        exports[symbol_name] = item;
    }
}

void Segment::importSymbol(const std::string& local_name, std::string symbol_name, std::string seg_name,
                           Location &loc, AsmState &state) {
    if (hasSymbol(local_name)) {
        throw CasmErrorException("Symbol name all ready defined.", loc, state.currentLine->lineText);
    }
    imports[local_name] = ImportDeclaration{std::move(symbol_name), std::move(seg_name)};
}

bool Segment::hasSymbol(const std::string& sym_name) {
    return labelTable.hasLabel(sym_name) || (symbols.count(sym_name) == 1) || (importRefs.count(sym_name) == 1);
}

void Segment::enterSection(AlignType section_alignment, const Location &loc, const string &line) {
    unsigned start_offset;
    if (currentSection != nullptr) {
        if (currentSection->isBlock) {
            auto prev_section = section.rbegin() + 1;
            start_offset = prev_section->startAddress + prev_section->size + currentSection->size;
        }
        else {
            start_offset = currentSection->startAddress + currentSection->size;
        }
    }
    else {
        start_offset = 0;
    }
    unsigned test_mask;
    unsigned and_mask;
    unsigned increment;
    switch (section_alignment) {
        case AlignType::byte:
            test_mask = 0x0000;
            and_mask = 0xFFFF;
            increment = 0;
            break;
        case AlignType::word:
            test_mask = 0x0001;
            and_mask = 0xFFFE;
            increment = 2;
            break;
        case AlignType::dword:
            test_mask = 0x0003;
            and_mask = 0xFFFC;
            increment = 4;
            break;
        case AlignType::para:
            test_mask = 0x000F;
            and_mask = 0xFFF0;
            increment = 16;
            break;
        case AlignType::page:
            test_mask = 0x00FF;
            and_mask = 0xFF00;
            increment = 256;
            break;
    }
    if ((start_offset & test_mask) != 0) {
        start_offset = (start_offset + increment) & and_mask;
    }
    if (start_offset > 0xFFFF) {
        throw CasmErrorException("Segment wrap around.", loc, line);
    }
    section.push_back({static_cast<int>(start_offset), section_alignment, nullptr, 0, false});
    currentSection = &section.back();
}



void Segment::assignSymbol(string &symbol_name, AsmState &state) {
    if (labelTable.hasLabel(symbol_name) || symbols.count(symbol_name) > 0 ||
        variables.count(symbol_name) > 0 || imports.count(name) > 0) {
        throw CasmErrorException("Segment already has a symbol by this name defined.",
                                 state.currentLine->labelLoc, state.currentLine->lineText);
    }
    symbols[symbol_name] = Symbol();
}

void Segment::enterBlock(int block_address) {
    section.push_back({block_address, AlignType::byte, nullptr, 0, true});
    currentSection = &section.back();
}

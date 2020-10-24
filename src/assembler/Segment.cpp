//
// Created by nathan on 7/22/20.
//

#include <utility>
#include <string>
#include <stack>

#include "Segment.h"
#include "AsmState.h"
#include "Line.h"
#include "Error.h"

using namespace std;

bool Segment::resolveSymbol(std::string symbol_name, Value &value) {
    bool is_absolute;
    if (symbol_name.back() == ':') {
        if (labelTable.findLocal(name, getOffset(), value.value, is_absolute)) {
            value.type = is_absolute ? ValueType::absolute : ValueType::relocatable;
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
    if (labelTable.findLabel(symbol_name, value.value, is_absolute)) {
        value.type = is_absolute ? ValueType::absolute : ValueType::relocatable;
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
        value.type = ValueType::relocatable;
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
                           Location &loc) {
    if (hasSymbol(local_name)) {
        throw CasmErrorException("Symbol name all ready defined.", loc,
                                 state->getCurrentLine()->lineText);
    }
    imports[local_name] = ImportDeclaration{std::move(symbol_name), std::move(seg_name)};
}

bool Segment::hasSymbol(const std::string& sym_name) {
    if (sym_name.back() == ':') {
        return labelTable.hasLocal(sym_name, currentOffset);
    }
    if (sym_name.back() == '&') {
        return variables.count(sym_name) > 0;
    }
    return labelTable.hasLabel(sym_name) || (symbols.count(sym_name) > 0) || (imports.count(sym_name) > 0);
}

void Segment::enterSection(AlignType section_alignment, const Location &loc, const string &line) {
    unsigned start_offset;
    if (currentSection != sections.end()) {
        if (currentSection->isBlock) {
            auto prev_section = currentSection - 1;
            start_offset = prev_section->startAddress + prev_section->size + currentSection->size;
        }
        else {
            start_offset = currentSection->startAddress + currentSection->size;
        }
    }
    else {
        start_offset = baseAddress;
    }
    if (!isPass2) {
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

        sections.push_back({static_cast<int>(start_offset), section_alignment, {}, 0, false});
    }
    currentSection = currentSection == sections.end() ? sections.begin() : currentSection + 1;
    currentOffset = isPass2 ? currentSection->startAddress : static_cast<int>(start_offset);
}



void Segment::assignSymbol(string &symbol_name, Value &value) {
    if (!isPass2 && (labelTable.hasLabel(symbol_name) || symbols.count(symbol_name) > 0 ||
        imports.count(symbol_name) > 0)) {
        auto currentLine = state->getCurrentLine();
        throw CasmErrorException("Segment already has a symbol by this name defined.",
                                 currentLine->labelLoc, currentLine->lineText);
    }
    symbols[symbol_name] = {name, value};
}

void Segment::enterBlock(int block_address) {
    if (!isPass2) {
        sections.push_back({block_address, AlignType::byte, {}, 0, true});
    }
    currentSection++;
    currentOffset = block_address;
}

void Segment::defineLabel(string &label_name) {
    auto asm_line = state->getCurrentLine();
    bool var_abs = isAbsolute || currentSection->isBlock;
    if (asm_line->isLocalLabel()) {
        if (labelTable.hasLocal(label_name, currentOffset)) {
            throw CasmErrorException("Local label already exists.", asm_line->labelLoc, asm_line->lineText);
        }
        labelTable.addLabel(label_name, true, currentOffset, var_abs);
    }
    else if (asm_line->isVariable()) {
            Value sym_value = {currentOffset, var_abs ? ValueType::absolute : ValueType::relocatable};
            variables[name] = {label_name, sym_value};
    }
    else {
        if (labelTable.hasLabel(label_name) || symbols.count(label_name) > 0 ||
            imports.count(label_name) > 0) {
            throw CasmErrorException("Segment already has a symbol by this name defined.",
                                     asm_line->labelLoc, asm_line->lineText);
        }
        labelTable.addLabel(label_name, false, currentOffset, var_abs);
    }
}

void Segment::allocateSpace(int size) {
    auto current_line = state->getCurrentLine();
    if (currentOffset + size > 0xFFFF) {
        throw CasmErrorException("Segment wrap around.",
                                 current_line->labelLoc, current_line->lineText);
    }
    currentOffset += size;
}

void Segment::endSegment() {
    auto current_line = state->getCurrentLine();
    if (inBlock()) {
        throw CasmErrorException("ENDS encountered while in a sub module block.",
                                 current_line->labelLoc, current_line->lineText);
    }
    if (state->isPassTwo) {
        if (currentSection->size != currentOffset - currentSection->startAddress) {
            throw CasmErrorException("Phase error - segment section size mismatch.",
                                     current_line->instructionLoc, current_line->lineText);
        }
    }
    else {
        currentSection->size = currentOffset - currentSection->startAddress;
    }
}

void Segment::pass2Setup() {
    isPass2 = true;
    currentSection = sections.end();
}

void Segment::addRelocationEntry(const Value& value, int operand_size, const Location& loc, const string& line_text) {
    if (!value.external && value.type == ValueType::absolute) return;

    if (value.external) {
        if (operand_size != 1 && (value.type == ValueType::relocatable_high ||
                                  value.type == ValueType::relocatable_low ||
                                  value.type == ValueType::segment)) {
            printWarnMsg("Expression does not match operand size.", loc, line_text);
        }
        if (operand_size == 1 && value.type == ValueType::relocatable) {
            printWarnMsg("Expression larger than byte operand.", loc, line_text);
        }
        if (operand_size == 3) {
            importRefs.push_back({currentOffset, ValueType::big, value.externalSeg, value.externalName});
        }
        else {
            importRefs.push_back({currentOffset, value.type, value.externalSeg, value.externalName});
        }
    } else {
        if (value.type == ValueType::segment) {
            if (operand_size > 1) {
                throw CasmErrorException("Segment reference used in a multi-byte operand.", loc, line_text);
            }
            currentSection->segmentRefs.push_back(currentOffset);
        }
        else {
            int offset = currentOffset;
            if (operand_size == 3) {
                currentSection->segmentRefs.push_back(currentOffset);
                offset++;
            }
            RelocationTypes rel_type = RelocationTypes::address;
            if (operand_size == 1) {
                if (value.type == ValueType::relocatable_low) {
                    rel_type = RelocationTypes::low_byte;
                } else if (value.type == ValueType::relocatable_high) {
                    rel_type = RelocationTypes::high_byte;
                } else {
                    printWarnMsg("Expression larger than byte operand.", loc, line_text);
                    rel_type = RelocationTypes::low_byte;
                }
            }
            currentSection->relocationTable.push_back({offset, value.baseValue, rel_type});
        }
    }
}

SegmentHeader Segment::getSegmentHeader(StringTable &string_table) {
    auto sh = SegmentHeader();
    sh.segmentNameStr = string_table.addString(name);
    sh.baseAddress = baseAddress;
    sh.type = static_cast<int>(type);
    sh.alignment = static_cast<int>(alignType);
    sh.exportCount = exports.size();
    sh.importCount = importRefs.size();
    sh.sectionCount = sections.size();
    return sh;
}

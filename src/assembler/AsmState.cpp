//
// Created by nathan on 7/27/20.
//

#include "AsmState.h"
#include "Segment.h"
#include "Expression.h"
#include "Line.h"
#include "Location.h"
#include "Error.h"

using namespace std;

bool AsmState::resolveSymbol(const string& symbol_name, Value &value, Location &loc) {
    auto dot_loc = symbol_name.find('.');
    if (dot_loc == string::npos && currentSegment != nullptr) {
        bool success = currentSegment->resolveSymbol(symbol_name, value, *this);
        if (!success && isPassTwo) {
            throw CasmErrorException("Symbol not found.", loc, currentLine->lineText);
        }
        return success;
    }
    if (symbol_name.back() == ':' || symbol_name.back() == '&') {
        throw CasmErrorException("Locals and variables referenced outside of a segment.",
                                 loc, currentLine->lineText);
    }
    string seg_name;
    string offset_name;
    if (dot_loc == 0 || dot_loc == string::npos) {
        offset_name = (dot_loc == 0) ? symbol_name.substr(1) : symbol_name;
    }
    else {
        seg_name = symbol_name.substr(0, dot_loc);
        if (dot_loc + 1 < symbol_name.length()) {
            offset_name = symbol_name.substr(dot_loc + 1);
        }
    }

    if (!seg_name.empty()) {
        auto seg_iter = segments.find(seg_name);
        if (seg_iter == segments.end()) {
            if (isPassTwo) {
                throw CasmErrorException("Undefined segment referenced.", loc, currentLine->lineText);
            }
            return false;
        }
        if (!offset_name.empty()) {
            bool success = seg_iter->second.resolveSymbol(offset_name, value, *this);
            if (success && value.isRelocatable()) {
                if (currentSegment == nullptr) {
                    throw CasmErrorException("Cannot import an exportable to global scope.",
                                             loc, currentLine->lineText);
                }
                seg_iter->second.exportSymbol(offset_name, value.value);
                value.external = true;
                value.externalName = offset_name;
                value.externalSeg = seg_name;
                value.value = 0;
                value.type = currentLine->hasLong ? ValueType::big : ValueType::relocatable;
            }
            if (!success && isPassTwo) {
                throw CasmErrorException("Symbol not found.", loc, currentLine->lineText);
            }
            return success;
        }
        value.type = ValueType::relocatable;
        value.external = true;
        value.externalSeg = seg_name;
        value.externalName.clear();
        value.value = 0;
        return true;
    }
    else {
        if (globals.count(offset_name) == 0) {
            if (isPassTwo) {
                throw CasmErrorException("Symbol undefined.", loc, currentLine->lineText);
            }
            else {
                return false;
            }
        }
        value = globals[offset_name].value;
        return true;
    }
}

int AsmState::getCurrentOffset(Location &loc) {
    if (currentSegment == nullptr) {
        throw CasmErrorException("Location counter not available in global scope",
                                 loc, currentLine->lineText);
    }
    return currentSegment->getOffset();
}

void AsmState::pass1(Line &line) {
    currentLine = &line;
    if (line.lineType == LineTypes::segment || line.lineType == LineTypes::variable) {
        line.instruction->pass1(line, *this);
    }
}

void AsmState::enterSegment(string &name, SegmentType &seg_type, AlignType &align_type) {
    int base_address = 0;
    if (currentSegment != nullptr) {
        throw CasmErrorException("Currently in a segment.",
                                 currentLine->instructionLoc, currentLine->lineText);
    }
    if (!currentLine->expressionList.empty()) {
        auto value = currentLine->expressionList.front().exp.getValue(*this);
        if (!value) {
            throw CasmErrorException("Expression for segment base unknown in pass 1.",
                                     currentLine->expressionList.front().loc, currentLine->lineText);
        }
        if (value->type != ValueType::absolute || value->external) {
            throw CasmErrorException("Expression for segment base must not be relocatable.",
                                     currentLine->expressionList.front().loc, currentLine->lineText);
        }
        base_address = value->value;
    }
    if (segments.count(name) == 0) {
        segments[name] = Segment(name, base_address, seg_type, align_type);
    }
    currentSegment = &segments[name];
    currentSegment->enterSection(align_type, currentLine->instructionLoc, currentLine->lineText);
}

void AsmState::assignSymbol(string &name) {
    if (currentSegment == nullptr) {
        if (globals.count(name) > 0) {
            throw CasmErrorException("Global symbol already defined.",
                                     currentLine->labelLoc, currentLine->lineText);
        }
        globals[name] = Symbol();
    }
    else {
        currentSegment->assignSymbol(name, *this);
    }

}

void AsmState::doAlignment(AlignType &align_type, Location &loc) {
    currentSegment->enterSection(align_type, loc, currentLine->lineText);
}

void AsmState::endBlock() {
    currentSegment->enterSection(AlignType::byte,
                                 currentLine->instructionLoc, currentLine->lineText);
}

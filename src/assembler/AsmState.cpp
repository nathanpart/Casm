//
// Created by nathan on 7/27/20.
//

#include "AsmState.h"
#include "Segment.h"
#include "Expression.h"

bool AsmState::resolveSymbol(const string& symbol_name, Value &value, int line, int col) {
    auto dot_loc = symbol_name.find('.');
    if (dot_loc == string::npos && currentSegment != nullptr) {
        bool success = currentSegment->resolveSymbol(symbol_name, value);
        if (!success && isPassTwo) {
            throw CasmErrorException("Symbol not found.", line, col);
        }
        return success;
    }
    if (symbol_name.back() == ':' || symbol_name.back() == '&') {
        throw CasmErrorException("Locals and variables referenced outside of a segment.",
                line, col);
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
                throw CasmErrorException("Undefined segment referenced.", line, col);
            }
            return false;
        }
        if (!offset_name.empty()) {
            bool success = seg_iter->second.resolveSymbol(offset_name, value);
            if (success && value.isRelocatable()) {
                if (currentSegment == nullptr) {
                    throw CasmErrorException("Cannot import an exportable to global scope.", line, col);
                }
                seg_iter->second.exportSymbol(offset_name, value.value);
                value.external = true;
                value.externalName = offset_name;
                value.externalSeg = seg_name;
                value.value = 0;
                value.type = isLong ? ValueType::big : ValueType::relocatable;
            }
            if (!success && isPassTwo) {
                throw CasmErrorException("Symbol not found.", line, col);
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
                throw CasmErrorException("Symbol undefined.", line, col);
            }
            else {
                return false;
            }
        }
        value = globals[offset_name].value;
        return true;
    }
}

int AsmState::getCurrentLocation(int line, int col) {
    if (currentSegment == nullptr) {
        throw CasmErrorException("Location counter not available in global scope", line, col);
    }
    return currentSegment->getLocation();
}

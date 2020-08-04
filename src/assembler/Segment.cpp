//
// Created by nathan on 7/22/20.
//

#include <utility>

#include "Segment.h"
#include "Assembler.h"
#include "AsmState.h"
#include "Line.h"


bool Segment::resolveSymbol(std::string symbol_name, Value &value, AsmState &state) {
    if (symbol_name.back() == ':') {
        if (labelTable.findLocal(name, getLocation(), value.value)) {
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
        int line, int col) {
    if (hasSymbol(local_name)) {
        throw CasmErrorException("Symbol name all ready defined.", line, col);
    }
    imports[local_name] = ImportDeclaration{std::move(symbol_name), std::move(seg_name)};
}

bool Segment::hasSymbol(const std::string& sym_name) {
    return labelTable.hasLabel(sym_name) || (symbols.count(sym_name) == 1) || (importRefs.count(sym_name) == 1);
}

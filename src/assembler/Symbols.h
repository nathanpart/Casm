//
// Created by nathan on 7/22/20.
//

#ifndef CASM_SYMBOLS_H
#define CASM_SYMBOLS_H

#include <string>
#include <vector>

#include "Expression.h"

struct Symbol {
    std::string name;
    Value value = {0, ValueType::unknown};
};


struct SymbolXref {
    std::string name;
    int definedLoc;
    std::vector<int> useLocations;
};

#endif //CASM_SYMBOLS_H

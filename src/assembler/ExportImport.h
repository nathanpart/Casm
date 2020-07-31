//
// Created by nathan on 7/22/20.
//

#ifndef CASM_EXPORTIMPORT_H
#define CASM_EXPORTIMPORT_H

#include <string>

#include "Expression.h"

struct ImportItem {
    int location;
    ValueType type;
    std::string segmentName;
    std::string name;
};

struct ImportDeclaration {
    std::string name;
    std::string segmentName;
};


struct ExportItem {
    std::string name;
    std::string segmentName;
    int location;
};


#endif //CASM_EXPORTIMPORT_H

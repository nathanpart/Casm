//
// Created by nathan on 8/4/20.
//

#ifndef CASM_LOCATION_H
#define CASM_LOCATION_H

#include <string>


struct Location {
    std::string locationName;
    int lineNumber;
    int column;
};


#endif //CASM_LOCATION_H

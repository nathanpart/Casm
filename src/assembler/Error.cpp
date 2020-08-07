//
// Created by nathan on 8/6/20.
//

#include <iostream>

#include "Error.h"

using namespace std;

void printErrorMsg(const string& msg, const Location& location, const string& lineText) {
    cerr << "Error:" << msg << "\n";

    if (!lineText.empty()) {
        cerr << lineText << "\n";
        for (int i = 0; i < location.column; i++)
            cerr << " ";
        cerr << "^~~\n";
    }
    cerr << location.locationName << " - Line: " << location.lineNumber << " Column: " << location.column << "\n\n";
}

//
// Created by nathan on 7/20/20.
//

#ifndef CASM_ASSEMBLER_H
#define CASM_ASSEMBLER_H


#include <string>
#include <stack>
#include <memory>
#include <tuple>
#include <utility>
#include <exception>
#include <vector>
#include <set>

#include "../Parser/tokenizer.h"
#include "../Parser/node.h"
#include "Line.h"
#include "Location.h"
#include "Macro.h"
#include "SourceItem.h"


class AsmState;
struct Value;


class Assembler {
    std::stack<SourceItem> sourceStack;

    int errorCount = 0;
    int errorLimit = 10;

    std::unique_ptr<AsmState> state;
    std::vector<Line> lines;
    Macro macros;
    std::set<std::string> includeList;

public:
    Assembler();
    void assemble(const std::string& file_name);
    void pre_process(std::ifstream &source_stream, const std::string& name);
    void pass1();
    void pass2();
    void writeObjectFile(const std::string &obj_file_name);
};


#endif //CASM_ASSEMBLER_H

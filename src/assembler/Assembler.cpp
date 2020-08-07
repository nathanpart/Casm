//
// Created by nathan on 7/20/20.
//

#include <fstream>

#include "Assembler.h"
#include "AsmState.h"
#include "Line.h"

#include "../Parser/graminit.h"
#include "SourceItem.h"
#include "Error.h"
#include "MacroLine.h"
#include "IncludeLine.h"

using namespace std;


Assembler::Assembler() {
    state = make_unique<AsmState>();
}


void Assembler::assemble(const string& file_name) {
    ifstream source = ifstream(file_name);
    pre_process(source, file_name);
}

void Assembler::pre_process(ifstream &source_stream, const string& name) {
    Line current_line;
    sourceStack.push(SourceItem(source_stream, name, SourceType::MAIN_FILE));

    while (!sourceStack.empty()) {
        try {
            shared_ptr<node> tree;
            string line_text;

            tie(tree, line_text) = sourceStack.top().getParsedLine();
            current_line = Line();
            current_line.location = sourceStack.top().getLocation();
            current_line.lineText = line_text;
            if (current_line.fromTree(*tree)) {
                if (current_line.lineType == LineTypes::macro) {
                    auto macro_line = dynamic_cast<MacroLine *>(current_line.instruction.get());
                    if (macro_line->isDef()) {
                        macros.addMacro(sourceStack.top(), macro_line->getName());
                    } else {
                        Location mac_loc = sourceStack.top().getLocation();
                        auto *loc_ptr = &mac_loc;

                        string mac_lines = macros.expandMacro(loc_ptr, macro_line->getName(),
                                                              macro_line->getArgs());
                        auto macroSource = SourceItem(mac_lines, loc_ptr->locationName,
                                                      SourceType::MACRO_EXPANSION, loc_ptr->lineNumber);
                        sourceStack.push(macroSource);
                    }
                }
                else if (current_line.lineType == LineTypes::include) {
                    auto include_line = dynamic_cast<IncludeLine *>(current_line.instruction.get());
                    string file_name = include_line->getName();
                    if (includeList.count(file_name) > 0) {
                        throw CasmErrorException("Recursive include file detected.",
                                                 sourceStack.top().getLocation(), line_text);
                    }
                    includeList.insert(file_name);
                    ifstream include_file = ifstream(file_name);
                    if (include_file.fail()) {
                        throw CasmErrorException("Unable to open include file.",
                                                 sourceStack.top().getLocation(), line_text);
                    }
                    auto include_source = SourceItem(include_file, file_name, SourceType::INCLUDE);
                    sourceStack.push(include_source);
                }
                else {
                    lines.push_back(current_line);
                }
            } else {
                if (sourceStack.top().getType() == SourceType::INCLUDE) {
                    includeList.erase(sourceStack.top().getName());
                }
                sourceStack.pop();
            }
        }
        catch (CasmErrorException &ce) {
            printErrorMsg(ce.what(), ce.getLocation(), ce.getLine());
            errorCount++;
            if (errorCount > errorLimit)
                return;
        }
    }
}

void Assembler::pass1() {
    for (auto &line: lines) {

    }
}

void Assembler::pass2() {

}



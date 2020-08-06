//
// Created by nathan on 7/20/20.
//

#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

#include "Assembler.h"
#include "AsmState.h"
#include "Line.h"

#include "../Parser/Parser.h"
#include "../Parser/errcode.h"
#include "../Parser/graminit.h"
#include "MacroLine.h"
#include "IncludeLine.h"


using namespace std;

SourceItem::SourceItem(ifstream &file, const string &fileName, SourceType type) {
    location.locationName = fileName;
    location.lineNumber = 0;
    location.column = 0;
    sourceType = type;
    sourceStream = make_unique<istream>(file.rdbuf());
    sourceTokenizer = make_unique<tokenizer>(*sourceStream);
}

SourceItem::SourceItem(const string& source, string sourceTitle, SourceType type, int firstLine) {
    location.locationName = move(sourceTitle);
    location.lineNumber = firstLine;
    location.column = 0;
    sourceType = type;
    sourceStream = make_unique<istream>(istringstream(source).rdbuf());
    sourceTokenizer = make_unique<tokenizer>(*sourceStream);
}

tuple<shared_ptr<node>, string> SourceItem::getParsedLine() {
    Parser parser;
    short token_type;
    string token_string;
    string line_string;
    int error_code;

    location.lineNumber++;
    while (true) {
        error_code = sourceTokenizer->getToken(token_type, token_string, line_string, location.column);
        if (error_code != E_OK) {
            break;
        }
        error_code = parser.addToken(token_type, token_string, location);
        if (error_code != E_OK) {
            if (error_code == E_DONE)
                error_code = E_OK;
            break;
        }
    }
    if (error_code != E_OK) {
        sourceTokenizer->endLine();
        switch(error_code) {
            case E_STREAM:
                throw CasmErrorException("Unable to read from source file.", location);
            case E_EOL:
                throw CasmErrorException("End of line reached before finding closing delimiter.", location);
            case E_TOKEN:
                throw CasmErrorException("Unrecognized symbol found.", location);
            default:
                throw CasmErrorException("Syntax error.", location);
        }
    }
    return make_tuple(parser.getTree(), line_string);
}

std::string SourceItem::getLine() {
    eofFlag = false;
    string line_text;
    int err = E_OK;
    sourceTokenizer->endLine();           // Make sure tokenizer will start from a fresh line for the next token
    location.lineNumber++;
    if (!getline(*sourceStream, line_text)) {
        err = sourceStream->eof() ? E_EOF : E_STREAM;
        if (err == E_STREAM) {
            throw CasmErrorException("Unable to read from source file.", location);
        }
        else {
            eofFlag = true;
        }
    }

    return line_text;
}


void Assembler::assemble() {
    if (objectFilename.empty()) {
        objectFilename = sourceFilename + ".co";
    }

    sourceStream = make_unique<ifstream>(sourceFilename);
    if (sourceStream->fail()) {
        cerr << "Unable to open source file: " << sourceFilename << endl;
        return;
    }

    objectStream = make_unique<ofstream>(objectFilename, ios::binary);
    if (objectStream->fail()) {
        cerr << "Unable to open object file: " << objectFilename << endl;
        return;
    }

    if (!listingFilename.empty()) {
        listingStream = make_unique<ofstream>(listingFilename);
        if (listingStream->fail()) {
            cerr << "Unable to open listing file: " << listingFilename << endl;
            return;
        }
        writeListing = true;
    }

    if (!xrefFilename.empty()) {
        xrefStream = make_unique<ofstream>(xrefFilename);
        if (xrefStream->fail()) {
            cerr << "Unable to open cross reference file: " << xrefFilename << endl;
            return;
        }
        writeXref = true;
    }


}

void Assembler::pre_process() {
    Line current_line;
    sourceStack.push(SourceItem(*sourceStream, sourceFilename, SourceType::MAIN_FILE));

    while (!sourceStack.empty()) {
        try {
            shared_ptr<node> tree;
            string line_text;
            string name;

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
                                                 sourceStack.top().getLocation());
                    }
                    includeList.insert(file_name);
                    ifstream include_file = ifstream(file_name);
                    if (include_file.fail()) {
                        throw CasmErrorException("Unable to open include file.",
                                                 sourceStack.top().getLocation());
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
            auto msg = ce.what();
            auto where = ce.getLocation();
            errorCount++;
            if (errorCount > errorLimit)
                return;
        }
    }
}

void Assembler::pass2() {

}


Assembler::Assembler(std::string sourceName, std::string objectName, std::string listingName, std::string xrefName)
        : sourceFilename(std::move(sourceName)), objectFilename(std::move(objectName)),
          listingFilename(std::move(listingName)), xrefFilename(std::move(xrefName)) {
    state = make_unique<AsmState>();

}


void printErrorMsg(const std::string& msg, const Location& loc, const string& lineText) {
    cerr << "Error:" << msg << "\n";

    cerr << lineText << "\n";
    for (int i = 0; i < loc.column; i++)
        cerr << " ";
    cerr << "^~~\n";
    cerr << loc.locationName << " - Line: " << loc.lineNumber << " Column: " << loc.column << "\n\n";
}

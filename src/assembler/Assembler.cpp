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
#include "../Parser/Parser.h"
#include "../Parser/errcode.h"
#include "../Parser/graminit.h"


using namespace std;

SourceItem::SourceItem(std::ifstream &file, const string &fileName, SourceType type) {
    name = fileName;
    sourceType = type;
    lineNumber = 1;
    startColumn = 0;
    sourceStream = make_unique<istream>(file.rdbuf());
    sourceTokenizer = make_unique<tokenizer>(*sourceStream);
}

SourceItem::SourceItem(const string& source, string sourceTitle, SourceType type, int firstLine) {
    name = move(sourceTitle);
    sourceType = type;
    lineNumber = firstLine;
    startColumn = 0;
    sourceStream = make_unique<istream>(istringstream(source).rdbuf());
    sourceTokenizer = make_unique<tokenizer>(*sourceStream);
}

tuple<shared_ptr<node>, int, string, string> SourceItem::getParsedLine() {
    Parser parser;
    short token_type;
    string token_string;
    string line_string;
    int error_code;

    while (true) {
        error_code = sourceTokenizer->getToken(token_type, token_string, line_string, startColumn);
        if (error_code != E_OK) {
            break;
        }
        error_code = parser.addToken(token_type, token_string, lineNumber, startColumn);
        if (error_code != E_OK) {
            if (error_code == E_DONE)
                error_code = E_OK;
            break;
        }
    }
    if (error_code != E_OK) {
        string msg;
        switch(error_code) {
            case E_STREAM:
                cerr << "ERROR -- Unable to read from source file: " << name << "\n\n";
                break;
            case E_EOL:
                msg = "ERROR -- End of line reached finding closing delimiter in " + name + "\n";
                printErrorMsg(msg, lineNumber, startColumn, line_string);
                break;
            case E_TOKEN:
                msg = "ERROR -- Unrecognized symbol found in " + name + "\n";
                printErrorMsg(msg, lineNumber, startColumn, line_string);
                break;
            default:
                msg = "ERROR -- Syntax error in " + name + "\n";
                printErrorMsg(msg, lineNumber, startColumn, line_string);
        }
        sourceTokenizer->endLine();   // Recover by advancing to next line
    }
    return make_tuple(parser.getTree(), error_code, line_string, name);
}

tuple<std::string, int, int> SourceItem::getLine() {
    string line_text;
    int err = E_OK;
    sourceTokenizer->endLine();           // Make sure tokenizer will start from a fresh line for the next token
    if (!getline(*sourceStream, line_text)) {
        err = sourceStream->eof() ? E_EOF : E_STREAM;
        if (err == E_STREAM) {
            cerr << "ERROR -- Unable to read from source file: " << name << "\n\n";
        }
    }

    return make_tuple(line_text, err, lineNumber++);
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

void Assembler::pass1() {
    sourceStack.push(SourceItem(*sourceStream, sourceFilename, SourceType::MAIN_FILE));

    while (!sourceStack.empty()) {
        shared_ptr<node> tree;
        int error_code;
        int line_number;
        int next_line;
        int column;
        string line_text;
        string name;

        tie(tree, error_code, line_text, name) = sourceStack.top().getParsedLine();
        if (error_code != E_OK) {
            errorCount++;
            if (errorCount > errorLimit)
                return;
        }
        if (processTree(tree, name)) {
            sourceStack.pop();
        }
    }
}

void Assembler::pass2() {

}

bool Assembler::processTree(std::shared_ptr<node> tree, std::string name) {
    bool returnCode = tree->child.back().type == ENDMARKER;
    string labelName;
    bool localLabel = false;

    for (auto child: tree->child) {
        if (child.type == label) {
            labelName = child.child.front().str;  // label: NAME [':']
            if (child.child.size() > 1)
                localLabel = true;
        }
    }

    // Exit if blank line
    if (tree->child.size() == 1) {
        return returnCode;
    }

    // Fetch a label if one is present

}

Assembler::Assembler(std::string sourceName, std::string objectName, std::string listingName, std::string xrefName)
        : sourceFilename(std::move(sourceName)), objectFilename(std::move(objectName)),
          listingFilename(std::move(listingName)), xrefFilename(std::move(xrefName)) {
    state = make_unique<AsmState>(*this);

}

bool Assembler::resolveSymbol(std::string symbol_name, bool &is_relocatable, Value &value) {
    return false;
}

void printErrorMsg(const std::string& msg, int lineNumber, int column, const string& lineText) {
    cerr << msg << "\n";
    cerr << lineText << "\n";
    for (int i = 0; i < column - 2; i++)
        cerr << " ";
    cerr << "--^\n";
    cerr << "Line: " << lineNumber << " Column: " << column << "\n\n";
}

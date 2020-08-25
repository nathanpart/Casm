//
// Created by nathan on 8/6/20.
//

#include <fstream>
#include <sstream>

#include "SourceItem.h"
#include "../Parser/Parser.h"
#include "../Parser/errcode.h"
#include "Error.h"

using namespace std;

SourceItem::SourceItem(ifstream &file, const string &fileName, SourceType type) {
    location.locationName = fileName;
    location.lineNumber = 0;
    location.column = 0;
    sourceType = type;
    sourceStream = make_shared<istream>(file.rdbuf());
    sourceTokenizer = make_shared<tokenizer>(*sourceStream);
}

SourceItem::SourceItem(const string& source, string sourceTitle, SourceType type, int firstLine) {
    location.locationName = move(sourceTitle);
    location.lineNumber = firstLine;
    location.column = 0;
    sourceType = type;
    sourceStream = make_shared<istream>(istringstream(source).rdbuf());
    sourceTokenizer = make_shared<tokenizer>(*sourceStream);
}

SourceItem::SourceItem(const SourceItem& si) {
    location = si.location;
    sourceTokenizer = si.sourceTokenizer;
    sourceType = si.sourceType;
    sourceStream = si.sourceStream;
    eofFlag = si.eofFlag;
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
                throw CasmErrorException("End of line reached before finding closing delimiter.",
                                         location, line_string);
            case E_TOKEN:
                throw CasmErrorException("Unrecognized symbol found.", location, line_string);
            default:
                throw CasmErrorException("Syntax error.", location, line_string);
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



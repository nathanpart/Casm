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


enum class SourceType {MAIN_FILE, INCLUDE, MACRO_EXPANSION};

class SourceItem {
    Location location;
    std::unique_ptr<tokenizer> sourceTokenizer;
    SourceType sourceType;
    std::unique_ptr<std::istream> sourceStream;
    bool eofFlag = false;

public:
    SourceItem(std::ifstream &file, const std::string &fileName, SourceType type);
    SourceItem(const std::string& source, std::string sourceTitle, SourceType type, int firstLine = 1);

    // returns parse_tree, error_code, line_text, source_name
    std::tuple<std::shared_ptr<node>, std::string> getParsedLine();

    // returns line_text, error_code, line_number
    std::string getLine();

    Location getLocation() {
        return location;
    }

    [[nodiscard]] bool atEof() const {
        return eofFlag;
    }

    [[nodiscard]] SourceType getType() const {
        return sourceType;
    }

    [[nodiscard]] std::string getName() const {
        return location.locationName;
    }

};

class CasmErrorException : public std::exception {
    const std::string error_msg;
    Location location;

public:
    CasmErrorException(std::string  err_mag, Location loc)
        : error_msg(std::move(err_mag)), location(std::move(loc)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return error_msg.c_str();
    }

    Location getLocation() {
        return location;
    }
};

class AsmState;
struct Value;

class Assembler {
    std::string sourceFilename;
    std::string objectFilename;
    std::string listingFilename;
    std::string xrefFilename;

    std::unique_ptr<std::ifstream> sourceStream;
    std::unique_ptr<std::ofstream> objectStream;
    std::unique_ptr<std::ofstream> listingStream;
    std::unique_ptr<std::ofstream> xrefStream;

    std::stack<SourceItem> sourceStack;
    bool writeListing = false;
    bool writeXref = false;

    int errorCount = 0;
    int errorLimit = 10;

    std::unique_ptr<AsmState> state;
    std::vector<Line> lines;
    Macro macros;
    std::set<std::string> includeList;

public:
    explicit Assembler(std::string sourceName, std::string objectName="",
            std::string listingName = "", std::string xrefName = "");
    void assemble();
    void pre_process();
    void pass2();
};

void printErrorMsg(const std::string& msg, int lineNumber, int column, const std::string& lineText);

#endif //CASM_ASSEMBLER_H

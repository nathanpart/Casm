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
#include "../Parser/tokenizer.h"
#include "../Parser/node.h"

enum class CpuType {CPU_6502, CPU_65C02, CPU_65C816};

enum class SourceType {MAIN_FILE, INCLUDE, MACRO_EXPANSION};

class SourceItem {
    std::unique_ptr<tokenizer> sourceTokenizer;
    SourceType sourceType;
    std::string name;
    std::unique_ptr<std::istream> sourceStream;
    int lineNumber;
    int startColumn;
public:
    SourceItem(std::ifstream &file, const std::string &fileName, SourceType type);
    SourceItem(const std::string& source, std::string sourceTitle, SourceType type, int firstLine = 1);

    // returns parse_tree, error_code, line_text, source_name
    std::tuple<std::shared_ptr<node>, int, std::string, std::string> getParsedLine();

    // returns line_text, error_code, line_number
    std::tuple<std::string, int, int> getLine();

    std::pair<int, int> getLocation() {
        return std::pair(lineNumber, startColumn);
    }
};

class CasmErrorException : public std::exception {
    const std::string error_msg;
    int lineNumber;
    int column;

public:
    CasmErrorException(std::string  err_mag, int lineNumber, int column)
        : error_msg(std::move(err_mag)), lineNumber(lineNumber), column(column) {}

    [[nodiscard]] const char* what() const noexcept override {
        return error_msg.c_str();
    }

    std::pair<int, int> getLocation() {
        return std::pair(lineNumber, column);
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

public:
    explicit Assembler(std::string sourceName, std::string objectName="",
            std::string listingName = "", std::string xrefName = "");
    void assemble();
    void pass1();
    void pass2();
    bool processTree(std::shared_ptr<node> tree, std::string name);
    bool resolveSymbol(std::string symbol_name, bool &is_relocatable, Value &value);
};

void printErrorMsg(const std::string& msg, int lineNumber, int column, const std::string& lineText);

#endif //CASM_ASSEMBLER_H

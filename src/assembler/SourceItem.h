//
// Created by nathan on 8/6/20.
//

#ifndef CASM_SOURCEITEM_H
#define CASM_SOURCEITEM_H

#include <string>
#include <memory>

#include "Location.h"
#include "../Parser/node.h"
#include "../Parser/tokenizer.h"

enum class SourceType {MAIN_FILE, INCLUDE, MACRO_EXPANSION};

class SourceItem {
    Location location;
    std::shared_ptr<tokenizer> sourceTokenizer;
    SourceType sourceType;
    std::shared_ptr<std::istream> sourceStream;
    bool eofFlag = false;

public:
    SourceItem(const std::string &fileName, SourceType type);
    SourceItem(const std::string& source, std::string sourceTitle, SourceType type, int firstLine = 1);
    SourceItem(const SourceItem& si);

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



#endif //CASM_SOURCEITEM_H

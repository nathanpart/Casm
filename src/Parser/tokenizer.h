//
// Created by nathan on 7/18/20.
//

#ifndef CASM_TOKENIZER_H
#define CASM_TOKENIZER_H

#include <istream>
#include <string>

class tokenizer {
    std::istream &in_stream;
    int column;
    std::string line;
    bool nextLine;
    bool file_end;


public:
    explicit tokenizer(std::istream &source)
            : in_stream(source), column(0), nextLine(true), file_end(false) {}

    int getToken(short &type, std::string &str, std::string &line_str, int &start_col);
    int fetchLine(short &type, std::string &str, std::string &line_str, int &start_col);
    void endLine() {
        nextLine = true;
    }


};


#endif //CASM_TOKENIZER_H

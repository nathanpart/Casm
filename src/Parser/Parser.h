//
// Created by nathan on 7/18/20.
//

#ifndef CASM_PARSER_H
#define CASM_PARSER_H

#include <stack>
#include <memory>
#include <string>

#include "grammar.h"
#include "node.h"

struct StackEntry {
    int                    i_state;
    const dfa             *i_dfa;
    node                  &i_parent;
    StackEntry(const dfa *dfa, node &n) : i_dfa(dfa), i_parent(n), i_state(0) {}
};

class ParserStack : public std::stack<StackEntry> {
public:
    int shift(short type, const std::string& str, int newState, Location loc);
    int ppush(short type, const dfa *d, int newState, Location loc);
};



class Parser {
    ParserStack             i_stack;
    grammar                &i_grammar;
    std::shared_ptr<node>   i_tree;

    int classify(short type, const char *str) const;

public:
    Parser();
    int addToken(short type, const std::string& str,  const Location& loc);
    std::shared_ptr<node> getTree() {
        return i_tree;
    }
};





#endif //CASM_PARSER_H

//
// Created by nathan on 7/15/20.
//

#ifndef CASM_NODE_H
#define CASM_NODE_H


#include <utility>
#include <vector>
#include <cassert>
#include <string>

class node {
public:
    short type;
    std::string str;
    int lineno;
    int col_offset;
    int num_children;
    std::vector<node> child;
    std::vector<int> foo;

    node() : type(0),  lineno(0), col_offset(0), num_children(0) {}
    explicit node(short type) : type(type), lineno(0), num_children(0), col_offset(0) {}

    node(short type, std::string str, int line_num, int col_offset, int num_children)
        : type(type), str(std::move(str)), lineno(line_num), col_offset(col_offset), num_children(num_children) {}

    [[nodiscard]] int get_num_children() const {
        return child.size();
    }

    [[nodiscard]] node &get_child(int i) {
        return child.at(i);
    }

    [[nodiscard]] node get_rchild(int i) const {
        return child.at(get_num_children() + i);
    }

    [[nodiscard]] short get_type() const {
        return type;
    }

    [[nodiscard]] std::string get_str() const {
        return str;
    }

    [[nodiscard]] int get_line_num() const {
        return lineno;
    }

    void req(short req_type) const {
        assert(req_type == type);
    }

    void listTree() const;
    int addChild(short type, std::string str, int line_num, int column_offset);
    void listTree();
};


#endif //CASM_NODE_H

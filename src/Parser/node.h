//
// Created by nathan on 7/15/20.
//

#ifndef CASM_NODE_H
#define CASM_NODE_H


#include <utility>
#include <vector>
#include <cassert>
#include <string>

#include "../assembler/Location.h"

class node {
public:
    short type;
    std::string str;
    Location location;
    int num_children;
    std::vector<node> child;
    std::vector<int> foo;

    node() : type(0), num_children(0) {}
    explicit node(short type) : type(type), num_children(0)  {}

    node(short type, std::string str, Location loc, int num_children)
        : type(type), str(std::move(str)), location(loc), num_children(num_children) {}

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

    [[nodiscard]] Location get_location() const {
        return location;
    }

    void req(short req_type) const {
        assert(req_type == type);
    }

    void listTree() const;
    int addChild(short type, std::string str, Location loc);
    void listTree();
};


#endif //CASM_NODE_H

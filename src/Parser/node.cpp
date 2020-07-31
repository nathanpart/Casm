//
// Created by nathan on 7/15/20.
//
#include <iostream>
#include <utility>
#include "node.h"

using namespace std;

void node::listTree() const {

}

int node::addChild(short child_type, std::string child_str, int line_num, int column_offset) {
    node n = node(child_type, std::move(child_str), line_num, column_offset, 0);
    child.push_back(n);
    return 0;
}

void node::listTree() {
    cout << "Dump the tree" << endl;
}

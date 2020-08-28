//
// Created by nathan on 7/15/20.
//
#include <iostream>
#include <utility>
#include "node.h"

using namespace std;

void node::listTree() const {

}

int node::addChild(short child_type, string child_str, Location loc) {
    node n = node(child_type, std::move(child_str), std::move(loc));
    child.push_back(n);
    return 0;
}

void node::listTree() {
    cout << "Dump the tree" << endl;
}

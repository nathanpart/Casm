//
// Created by nathan on 7/15/20.
//
#include <iostream>

#include <utility>
#include "node.h"
#include "token.h"
#include "grammar.h"

using namespace std;

void node::listTree() const {
    listTree(0);
}

int node::addChild(short child_type, string child_str, Location loc) {
    node n = node(child_type, std::move(child_str), std::move(loc));
    child.push_back(n);
    return 0;
}

void node::listTree(int level) const {
    for (int n = 0; n < level; n++) {
        cout << ' ';
    }
    if (type <= N_TOKENS) {
        cout << casmTokenNames[type];
        if (!str.empty()) {
            cout << "    " << str;
        }
        cout << endl;
     }
    else if (type >= 256) {
        cout << casmGrammar.findDFA(type)->name << endl;
        for (const auto& cn: child) {
            cn.listTree(level + 1);
        }
    }
    else {
        cout << "Unknown Node Type: " << type << endl;
    }

}

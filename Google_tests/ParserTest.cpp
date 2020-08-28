#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by nathan on 8/25/20.
//

#include <iostream>

#include "gtest/gtest.h"
#include "../src/Parser/Parser.h"
#include "../src/Parser/graminit.h"

TEST(Parser, TestBit) {
    const dfa *d1 = casmGrammar.findDFA(include);
    ASSERT_TRUE(d1->testBit(51));
    ASSERT_FALSE(d1->testBit(24));
}

TEST(Parser, Constructor) {
    Parser parser;
    auto tree = parser.getTree();
    ASSERT_EQ(tree->type, line);
}

TEST(Parser, AddToken) {
    Parser parser;
    parser.addToken(GROUP1_INST, "adc", {"foo", 1, 2});
    auto tree = parser.getTree();
    ASSERT_EQ(tree->child.size(), 1);
    ASSERT_EQ(tree->child.front().type, op_code);
    ASSERT_EQ(tree->child.front().child.size(), 1);
    ASSERT_EQ(tree->child.front().child.front().type, group1);
    ASSERT_EQ(tree->child.front().child.front().child.front().type, GROUP1_INST);
    ASSERT_TRUE(tree->child.front().child.front().child.front().child.empty());
}
#pragma clang diagnostic pop
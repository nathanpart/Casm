#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by nathan on 8/29/20.
//

#include <cstdio>
#include <string>
#include <fstream>

#include "gtest/gtest.h"
#include "../src/assembler/SourceItem.h"
#include "../src/Parser/graminit.h"
#include "../src/Parser/token.h"

using namespace std;

class SourceItemTest: public ::testing::Test {
protected:
    char *sample_source_name;
    std::string source_sample;

    void SetUp() override {
        source_sample = "* Sample\n";
        source_sample += "main   SEGMENT\n";
        source_sample += "start  lda  #'a'\n";
        source_sample += "       sta  $0400  ; Store 'A' into to the top left of screen.\n";
        source_sample += "       ENDS\n";

        sample_source_name = tempnam("/tmp", "casm");
        std::ofstream sample_file{ sample_source_name };
        sample_file << source_sample;
        sample_file.close();
    }

    void TearDown() override {
        unlink(sample_source_name);
        free(sample_source_name);
    }

};

TEST_F(SourceItemTest, SourceString) {
    SourceItem si(source_sample, "Sample", SourceType::INCLUDE);
    EXPECT_STREQ("Sample", si.getName().c_str());
    EXPECT_FALSE(si.atEof());

    auto [line_node, line_str] = si.getParsedLine();
    EXPECT_EQ(line, line_node->type);
    EXPECT_TRUE(!line_node->child.empty());
    EXPECT_EQ(1, line_node->child.size());
    node leaf = line_node->child.front();
    EXPECT_EQ(NEWLINE, leaf.type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(label, line_node->child[0].type);
    EXPECT_EQ(region, line_node->child[1].type);
    EXPECT_EQ(NEWLINE, line_node->child[2].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(label, line_node->child[0].type);
    EXPECT_EQ(op_code, line_node->child[1].type);
    EXPECT_EQ(NEWLINE, line_node->child[2].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(op_code, line_node->child[0].type);
    EXPECT_EQ(NEWLINE, line_node->child[1].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(region, line_node->child[0].type);
    EXPECT_EQ(NEWLINE, line_node->child[1].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(ENDMARKER, line_node->child[0].type);
}

TEST_F(SourceItemTest, SourceFile) {
    SourceItem si(sample_source_name, SourceType::INCLUDE);
    EXPECT_STREQ(sample_source_name, si.getName().c_str());
    EXPECT_FALSE(si.atEof());

    auto [line_node, line_str] = si.getParsedLine();
    EXPECT_EQ(line, line_node->type);
    EXPECT_TRUE(!line_node->child.empty());
    EXPECT_EQ(1, line_node->child.size());
    node leaf = line_node->child.front();
    EXPECT_EQ(NEWLINE, leaf.type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(label, line_node->child[0].type);
    EXPECT_EQ(region, line_node->child[1].type);
    EXPECT_EQ(NEWLINE, line_node->child[2].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(label, line_node->child[0].type);
    EXPECT_EQ(op_code, line_node->child[1].type);
    EXPECT_EQ(NEWLINE, line_node->child[2].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(op_code, line_node->child[0].type);
    EXPECT_EQ(NEWLINE, line_node->child[1].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(region, line_node->child[0].type);
    EXPECT_EQ(NEWLINE, line_node->child[1].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(ENDMARKER, line_node->child[0].type);
}

TEST_F(SourceItemTest, GetLine) {
    SourceItem si(source_sample, "Sample", SourceType::INCLUDE);
    EXPECT_STREQ("Sample", si.getName().c_str());
    EXPECT_FALSE(si.atEof());

    auto [line_node, line_str] = si.getParsedLine();
    EXPECT_EQ(line, line_node->type);
    EXPECT_TRUE(!line_node->child.empty());
    EXPECT_EQ(1, line_node->child.size());
    node leaf = line_node->child.front();
    EXPECT_EQ(NEWLINE, leaf.type);

    EXPECT_STREQ("main   SEGMENT", si.getLine().c_str());

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(label, line_node->child[0].type);
    EXPECT_EQ(op_code, line_node->child[1].type);
    EXPECT_EQ(NEWLINE, line_node->child[2].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(op_code, line_node->child[0].type);
    EXPECT_EQ(NEWLINE, line_node->child[1].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(region, line_node->child[0].type);
    EXPECT_EQ(NEWLINE, line_node->child[1].type);

    tie(line_node, line_str) = si.getParsedLine();
    EXPECT_EQ(ENDMARKER, line_node->child[0].type);
}

TEST_F(SourceItemTest, LineNumber) {
    SourceItem si(source_sample, "Sample", SourceType::INCLUDE, 0);
    Location loc;

    auto [line_node, line_str] = si.getParsedLine();
    loc = si.getLocation();
    EXPECT_EQ(1, loc.lineNumber);

    tie(line_node, line_str) = si.getParsedLine();
    loc = si.getLocation();
    EXPECT_EQ(2, loc.lineNumber);

    tie(line_node, line_str) = si.getParsedLine();
    loc = si.getLocation();
    EXPECT_EQ(3, loc.lineNumber);

    tie(line_node, line_str) = si.getParsedLine();
    loc = si.getLocation();
    EXPECT_EQ(4, loc.lineNumber);

    tie(line_node, line_str) = si.getParsedLine();
    loc = si.getLocation();
    EXPECT_EQ(5, loc.lineNumber);
}


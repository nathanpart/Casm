//
// Created by nathan on 9/25/20.
//

#include "gtest/gtest.h"
#include "../src/assembler/Label.h"

TEST(LabelTest, GlobalLabel) {
    Labels labels;
    int address;
    bool is_absolute;

    labels.addLabel("foo", false, 0xFF00, false);
    EXPECT_TRUE(labels.findLabel("foo", address, is_absolute));
    EXPECT_EQ(0xFF00, address);
    EXPECT_FALSE(is_absolute);
}

TEST(LabelTest, UnknownLabel) {
    Labels labels;
    int address;
    bool is_absolute;

    EXPECT_FALSE(labels.findLabel("foo", address, is_absolute));
}

TEST(LabelTest, GlobalLabels) {
    Labels labels;
    int address = 0;
    bool is_absolute = true;

    EXPECT_FALSE(labels.hasLabel("foo"));
    EXPECT_FALSE(labels.findLabel("foo", address, is_absolute));
    EXPECT_EQ(0, address);
    EXPECT_TRUE(is_absolute);
    labels.addLabel("foo", false, 0xFF00, false);
    EXPECT_TRUE(labels.hasLabel("foo"));
    EXPECT_TRUE(labels.findLabel("foo", address, is_absolute));
    EXPECT_EQ(0xFF00, address);
    EXPECT_FALSE(is_absolute);

    EXPECT_FALSE(labels.hasLabel("fee"));
    EXPECT_FALSE(labels.findLabel("fee", address, is_absolute));
    EXPECT_EQ(0xFF00, address);     // Theses values should still be the results of the lookup of "foo"
    EXPECT_FALSE(is_absolute);
    labels.addLabel("fee", false, 0xFF03, true);
    EXPECT_TRUE(labels.hasLabel("fee"));
    EXPECT_TRUE(labels.findLabel("fee", address, is_absolute));
    EXPECT_EQ(0xFF03, address);
    EXPECT_TRUE(is_absolute);

    EXPECT_TRUE(labels.hasLabel("foo"));
    EXPECT_TRUE(labels.findLabel("foo", address, is_absolute));
    EXPECT_EQ(0xFF00, address);
    EXPECT_FALSE(is_absolute);
}

TEST(LabelTest, LocalLabel) {
    Labels labels;
    int address = 0;
    bool is_absolute = true;

    EXPECT_FALSE(labels.hasLabel("foo"));
    EXPECT_FALSE(labels.findLabel("foo", address, is_absolute));
    EXPECT_EQ(0, address);
    EXPECT_TRUE(is_absolute);
    labels.addLabel("foo", true, 0x0005, false);
    EXPECT_TRUE(labels.hasLocal("foo", 0x0010));
    EXPECT_TRUE(labels.findLocal("foo", 0x0010, address, is_absolute));
    EXPECT_EQ(0x0005, address);
    EXPECT_FALSE(is_absolute);

    EXPECT_FALSE(labels.hasLabel("fee"));
    EXPECT_FALSE(labels.findLabel("fee", address, is_absolute));
    EXPECT_EQ(0x0005, address);     // Theses values should still be the results of the lookup of "foo"
    EXPECT_FALSE(is_absolute);
    labels.addLabel("fee", false, 0x0020, true);
    EXPECT_TRUE(labels.hasLabel("fee"));
    EXPECT_TRUE(labels.findLabel("fee", address, is_absolute));
    EXPECT_EQ(0x0020, address);
    EXPECT_TRUE(is_absolute);

    EXPECT_TRUE(labels.hasLocal("foo", 0x0010));
    EXPECT_FALSE(labels.hasLocal("foo", 0x025));
}

//
// Created by nathan on 10/26/20.
//

#include <string>
#include "AsmTestMoc.h"
#include "../assembler/SourceItem.h"
#include "../assembler/Expression.h"
#include "../Parser/graminit.h"

using namespace std;

TEST(ExpressionTest, Number) {
    string num_str = "lda #5\n";
    auto src_item = SourceItem(num_str, "Number", SourceType::INCLUDE);
    auto [line_node, line_str] = src_item.getParsedLine();
    // Navigate to the exp non-terminal
    //    line
    //      op_code
    //        group1
    //          GROUP1_INST    lda
    //          imm_exp
    //            HASH         #
    //            exp
    //            ...
    auto node = line_node->child.front().child.front().child.back().child.back();
    ASSERT_EQ(expr, node.type);

//    node.listTree();  // Uncomment to dump expression tree during debugging

    using ::testing::_;
    MockAsmState state;
    EXPECT_CALL(state, resolveSymbol(_, _, _))
        .Times(0);
    EXPECT_CALL(state, getCurrentOffset(_))
        .Times(0);

    Expression expression;
    expression.buildRpnList(node, line_str);
    auto exp_value = expression.getValue(state);
    EXPECT_TRUE(exp_value.has_value());
    EXPECT_EQ(5, exp_value.value().value);
}
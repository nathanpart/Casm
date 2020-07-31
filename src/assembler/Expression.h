//
// Created by nathan on 7/22/20.
//

#ifndef CASM_EXPRESSION_H
#define CASM_EXPRESSION_H

#include <variant>
#include <string>
#include <vector>
#include "../Parser/node.h"

enum class ValueType {absolute, relocatable, relocatable_high, relocatable_low, segment, big, unknown};
struct Value {
    int value;
    ValueType type;
    bool external;
    std::string externalName;
    std::string externalSeg;
    int baseValue;
    [[nodiscard]] bool isRelocatable() const {
        return type == ValueType::relocatable_high || type == ValueType::relocatable_low ||
                type == ValueType::segment || type == ValueType::relocatable || external;
    }

    [[nodiscard]] bool isRelocatableByte() const {
        return type == ValueType::relocatable_high || type == ValueType::relocatable_low || type == ValueType::segment;
    }
};


enum class ExpItemType {str, number, sym, cur_loc, op};
enum class OpType {plus, minus, multiply, divide, mod, positive, negative, log_not, low_byte, high_byte, seg_byte,
    shift_left, shift_right, log_and, log_xor, log_or, less, greater, equal, not_equal, greater_equal, less_equal,
    rel_not, rel_and, rel_or, low_word};

class AsmState;

struct ExpressionItem {
    ExpItemType type;
    OpType opType;
    Value value;
    std::string stringValue;
    int lineNumber;
    int column;
    [[nodiscard]] bool isRelocatable() const { return value.isRelocatable(); }
    [[nodiscard]] bool isRelocatableByte() const { return value.isRelocatableByte(); }
    [[nodiscard]] bool isAbsRelocatable() const { return isRelocatable() && !isRelocatableByte(); }
    [[nodiscard]] bool isInternalAbsRel() const { return isAbsRelocatable() && !value.external; }
};


class Expression {
    std::vector<ExpressionItem> rpnList;
    bool symbolsResolved = true;

    void evalTreeLevel(const node& expr_tree);
public:
    void buildRpnList(const node& expr_tree);
    std::variant<Value, std::string> evaluate(AsmState &state);
};


#endif //CASM_EXPRESSION_H

//
// Created by nathan on 7/22/20.
//

#include <regex>
#include <stack>

#include "Expression.h"
#include "../Parser/token.h"
#include "../Parser/graminit.h"
#include "Assembler.h"
#include "AsmState.h"
#include "Error.h"

using namespace std;

static void evaluateString(ExpressionItem &exp_item, string &line_text);

[[maybe_unused]] void Expression::buildRpnList(const node &expr_tree, string &line_text) {
    lineText = move(line_text);
    evalTreeLevel(expr_tree);
}

void Expression::evalTreeLevel(const node &expr_tree) {
    stack<ExpressionItem> op_stack;

    for (const auto& child_node: expr_tree.child) {
        if (ISNONTERMINAL(child_node.type)) {
            if (child_node.type == symbol) {
                string str;
                for (const auto& sym_token: child_node.child) {
                    switch (sym_token.type) {
                        case DOT:
                            str += '.';
                            break;
                        case COLON:
                            str += ':';
                            break;
                        case QUESTION:
                            str += '?';
                            break;
                        default:
                            str += sym_token.str;  // NAME
                    }
                }
                ExpressionItem op;
                op.type = ExpItemType::sym;
                op.stringValue = str;
                op.loc = child_node.location;
                rpnList.push_back(op);
            }
            else if (child_node.type == xs_index) {
                throw CasmErrorException("Unexpected index specifier in expression.",
                                         child_node.location, lineText);
            }
            else {
                evalTreeLevel(child_node);
                if (!op_stack.empty()) {
                    rpnList.push_back(op_stack.top());
                    op_stack.pop();
                }
            }
        }
        else {
            ExpressionItem op;
            op.type = ExpItemType::op;
            op.loc = child_node.location;
            Value num = {0, ValueType::absolute};
            string num_str;
            switch (child_node.type) {

                case DOUBLEVBAR:
                    op.opType = OpType::rel_or;
                    op_stack.push(op);
                    break;
                case DUUBLEAMPER:
                    op.opType = OpType::rel_and;
                    op_stack.push(op);
                    break;
                case EXCLAMATION:
                    op.opType = OpType::rel_not;
                    op_stack.push(op);
                    break;
                case LESS:
                    op.opType = (expr_tree.type == factor) ? OpType::low_byte : OpType::less;
                    op_stack.push(op);
                    break;
                case GREATER:
                    op.opType = (expr_tree.type == factor) ? OpType::high_byte : OpType::greater;
                    op_stack.push(op);
                    break;
                case EQEQUAL:
                    op.opType = OpType::equal;
                    op_stack.push(op);
                    break;
                case NOTEQUAL:
                    op.opType = OpType::not_equal;
                    op_stack.push(op);
                    break;
                case GREATEREQUAL:
                    op.opType = OpType::greater_equal;
                    op_stack.push(op);
                    break;
                case LESSEQUAL:
                    op.opType = OpType::less_equal;
                    op_stack.push(op);
                    break;
                case VBAR:
                    op.opType = OpType::log_or;
                    op_stack.push(op);
                    break;
                case CIRCUMFLEX:
                    op.opType = OpType::log_xor;
                    op_stack.push(op);
                    break;
                case AMPER:
                    op.opType = OpType::log_and;
                    op_stack.push(op);
                    break;
                case LEFTSHIFT:
                    op.opType = (expr_tree.type == factor) ? OpType::low_word : OpType::shift_left;
                    op_stack.push(op);
                    break;
                case RIGHTSHIFT:
                    op.opType = (expr_tree.type == factor) ? OpType::seg_byte : OpType::shift_right;
                    op_stack.push(op);
                    break;
                case PLUS:
                    op.opType = (expr_tree.type == factor) ? OpType::positive : OpType::plus;
                    op_stack.push(op);
                    break;
                case MINUS:
                    op.opType = (expr_tree.type == factor) ? OpType::negative : OpType::minus;
                    op_stack.push(op);
                    break;
                case STAR:
                    if (expr_tree.type == atom) {
                        op.type = ExpItemType::cur_loc;
                        rpnList.push_back(op);
                    } else {
                        op.opType = OpType::multiply;
                        op_stack.push(op);
                    }
                    break;
                case SLASH:
                    op.opType = OpType::divide;
                    op_stack.push(op);
                    break;
                case PERCENT:
                    op.opType = OpType::mod;
                    op_stack.push(op);
                    break;
                case TILDE:
                    op.opType = OpType::log_not;
                    op_stack.push(op);
                    break;
                case STRING:
                    op.type = ExpItemType::str;
                    op.stringValue = child_node.str;
                    evaluateString(op, lineText);
                    rpnList.push_back(op);
                    break;
                case NUMBER:
                    num_str = child_node.str;
                    try {
                        if (num_str[0] == '%') {
                            num.value = stoi(num_str.substr(2), nullptr, 2);
                        }
                        else if (num_str[0] == '$') {
                            num.value = stoi(num_str.substr(2), nullptr, 16);
                        }
                        else {
                            num.value = stoi(num_str, nullptr, 0);
                        }
                    }
                    catch (out_of_range &ex) {
                        throw CasmErrorException("Integer is too big.",
                                                 child_node.location, lineText);
                    }
                    op.type = ExpItemType::number;
                    op.value = num;
                    op.value.external = false;
                    op.value.type = ValueType::absolute;
                    break;
                default:
                    ;        // Only the ( and ) should land here and we don't need to do anything with them
            }
        }
    }

}

void Expression::validateChar(ExpressionItem &item) {
    if (item.type == ExpItemType::str) {
        if (item.stringValue.length() > 1) {
            throw CasmErrorException("Sting is not a single char in computation",
                                     item.loc, lineText);
        } else {
            item.value = {item.stringValue.front(), ValueType::absolute, false};
            item.type = ExpItemType::number;
        }
    }
}

void Expression::popBinaryArgs(stack<ExpressionItem> &exp_stack, ExpressionItem &lhs, ExpressionItem &rhs) {
    rhs = exp_stack.top();
    exp_stack.pop();
    lhs = exp_stack.top();
    exp_stack.pop();
}

void Expression::popUniArg(stack<ExpressionItem> &exp_stack, ExpressionItem &arg) {
    arg = exp_stack.top();
    exp_stack.pop();
}


void Expression::validateArguments(ExpressionItem &lhs, ExpressionItem& rhs, OpType opType) {
    bool lhs_external = lhs.value.external;
    bool rhs_external = lhs.value.external;

    validateChar(lhs);
    validateChar(rhs);

    if (lhs.isRelocatable() || rhs.isRelocatable()) {
        if (opType == OpType::minus) {
            if (lhs.isInternalAbsRel() && rhs.isInternalAbsRel()) {
                lhs.value.type = ValueType::absolute;
                return;
            }
            if (lhs.isAbsRelocatable() && !rhs.isRelocatable()) {
                return;
            }
        } else if (opType == OpType::plus) {
            if ((lhs.isAbsRelocatable() && !rhs.isRelocatable()) || (!lhs.isRelocatable() && rhs.isAbsRelocatable())) {
                return;
            }
        }
        Location loc = lhs.isRelocatable() ? lhs.loc : rhs.loc;
        throw CasmErrorException("Bad computation with an relocatable item.", loc, lineText);
    }
    if ((opType == OpType::divide || opType == OpType::mod) && rhs.value.value == 0)  {
        throw CasmErrorException("Trying to divide by 0.", rhs.loc, lineText);
    }
}

void Expression::popArgValidate(stack<ExpressionItem> &stack, ExpressionItem &lhs) {
    lhs = stack.top();
    stack.pop();
    if (lhs.isRelocatable()) {
        throw CasmErrorException("Bad computation with an relocatable item.", lhs.loc, lineText);
    }
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
void Expression::evaluate(AsmState &state) {
    stack<ExpressionItem> args;

    if (rpnList.size() == 1 && rpnList.front().type == ExpItemType::str) {
        evaluated = true;
        expString.emplace(rpnList.front().stringValue);
        expValue.reset();
    }
    auto pos = rpnList.begin();

    while (pos != rpnList.end()) {
        if (pos->type == ExpItemType::sym) {
            Value exp_value = {0, ValueType::unknown};
            if (!state.resolveSymbol(pos->stringValue, exp_value, pos->loc)) {
                evaluated = false;
                expString.reset();
                expValue.reset();
                return;
            }
            pos->type = ExpItemType::number;
            pos->value = exp_value;
        }
        if (pos->type == ExpItemType::cur_loc) {
            pos->type = ExpItemType::number;
            pos->value = {state.getCurrentOffset(pos->loc),
                          ValueType::relocatable, false};
        }
        if (pos->type != ExpItemType::op) {
            args.push(*pos);
            pos++;
            continue;
        }

        ExpressionItem rhs, lhs;
        switch (pos->opType) {
            case OpType::plus:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value += rhs.value.value;
                break;

            case OpType::minus:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value -= rhs.value.value;
                break;

            case OpType::multiply:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value *= rhs.value.value;
                break;

            case OpType::divide:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value /= rhs.value.value;
                break;

            case OpType::mod:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value %= rhs.value.value;
                break;

            case OpType::positive:
                popArgValidate(args, lhs);
                break;

            case OpType::negative:
                popArgValidate(args, lhs);
                lhs.value.value = -lhs.value.value;
                break;

            case OpType::log_not:
                popArgValidate(args, lhs);
                lhs.value.value = ~lhs.value.value;
                break;

            case OpType::low_byte:
                lhs = args.top();
                args.pop();
                if (lhs.isRelocatableByte()) {
                    throw CasmErrorException("Cannot apply byte operator to byte relocatable.",
                            lhs.loc, lineText);
                }
                if (lhs.isRelocatable()) {
                    lhs.value.baseValue = lhs.value.value;
                    lhs.value.type = ValueType::relocatable_low;
                }
                lhs.value.value &= 0xFF;
                if (lhs.value.type == ValueType::big) {
                    lhs.value.type = ValueType::absolute;
                }
                break;

            case OpType::high_byte:
                lhs = args.top();
                args.pop();
                if (lhs.isRelocatableByte()) {
                    throw CasmErrorException("Cannot apply byte operator to byte relocatable.",
                                             lhs.loc, lineText);
                }
                if (lhs.isRelocatable()) {
                    lhs.value.baseValue = lhs.value.value;
                    lhs.value.type = ValueType::relocatable_high;
                }
                lhs.value.value = (lhs.value.value >> 8) & 0xFF;
                if (lhs.value.type == ValueType::big) {
                    lhs.value.type = ValueType::absolute;
                }
                break;

            case OpType::seg_byte:
                lhs = args.top();
                args.pop();
                if (lhs.value.type != ValueType::big) {
                    throw CasmErrorException("Cannot apply seg byte operator a non big value.",
                                             lhs.loc, lineText);
                }
                if (lhs.isRelocatable()) {
                    lhs.value.baseValue = lhs.value.value;
                    lhs.value.type = ValueType::segment;
                }
                lhs.value.value = (lhs.value.value >> 16) & 0xFF;
                if (lhs.value.type == ValueType::big) {
                    lhs.value.type = ValueType::absolute;
                }
                break;

            case OpType::low_word:
                lhs = args.top();
                args.pop();
                if (lhs.value.type != ValueType::big) {
                    throw CasmErrorException("Cannot apply seg byte operator a non big value.",
                                             lhs.loc, lineText);
                }
                if (lhs.isRelocatable()) {
                    lhs.value.baseValue = lhs.value.value;
                    lhs.value.type = ValueType::relocatable;
                }
                lhs.value.value &=  0xFFFF;
                if (lhs.value.type == ValueType::big) {
                    lhs.value.type = ValueType::absolute;
                }
                break;


            case OpType::shift_left:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value <<= rhs.value.value;
                break;

            case OpType::shift_right:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value >>= rhs.value.value;
                break;

            case OpType::log_and:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value &= rhs.value.value;
                break;

            case OpType::log_xor:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value ^= rhs.value.value;
                break;

            case OpType::log_or:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value |= rhs.value.value;
                break;

            case OpType::less:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value < rhs.value.value;
                break;

            case OpType::greater:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value > rhs.value.value;
                break;

            case OpType::equal:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value == rhs.value.value;
                break;

            case OpType::not_equal:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value != rhs.value.value;
                break;

            case OpType::greater_equal:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value >= rhs.value.value;
                break;

            case OpType::less_equal:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value <= rhs.value.value;
                break;

            case OpType::rel_not:
                popArgValidate(args, lhs);
                lhs.value.value = !lhs.value.value;
                break;


            case OpType::rel_and:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value && rhs.value.value;
                break;

            case OpType::rel_or:
                popBinaryArgs(args, lhs, rhs);
                validateArguments(lhs, rhs, pos->opType);
                lhs.value.value = lhs.value.value || rhs.value.value;
                break;
        }
        args.push(lhs);
        pos++;
    }
    ExpressionItem ei = args.top();
    evaluated = true;
    expValue.emplace(ei.value);
    expString.reset();
}

ExpValue Expression::getValue(AsmState &state) {
    if (!evaluated) {
        evaluate(state);
    }
    return expValue;
}

ExpString Expression::getString(AsmState &state) {
    if (!evaluated) {
        evaluate(state);
    }
    return expString;
}

#pragma clang diagnostic pop


static pair<regex, char> * getControlCodesList() {

    static pair<regex, char> cbm_codes[] = {

            // Colors
            pair(regex(R"(^\{\s+blk\s*\})"), 0x90),
            pair(regex(R"(^\{\s+wht\s*\})"), 0x05),
            pair(regex(R"(^\{\s+red\s*\})"), 0x1C),
            pair(regex(R"(^\{\s+cyn\s*\})"), 0x9F),
            pair(regex(R"(^\{\s+pur\s*\})"), 0x9C),
            pair(regex(R"(^\{\s+grn\s*\})"), 0x1E),
            pair(regex(R"(^\{\s+blu\s*\})"), 0x1F),
            pair(regex(R"(^\{\s+yel\s*\})"), 0x9E),
            pair(regex(R"(^\{\s+orange\s*\})"), 0x81),
            pair(regex(R"(^\{\s+brown\s*\})"), 0x95),
            pair(regex(R"(^\{\s+lt red\s*\})"), 0x96),
            pair(regex(R"(^\{\s+dk gry\s*\})"), 0x97),
            pair(regex(R"(^\{\s+md gry\s*\})"), 0x98),
            pair(regex(R"(^\{\s+lt grn\s*\})"), 0x99),
            pair(regex(R"(^\{\s+lt blu\s*\})"), 0x9A),
            pair(regex(R"(^\{\s+lt gry\s*\})"), 0x9B),

            // Shift Enter, shift space
            pair(regex(R"(^\{\s+shift ret\s*\})"), 0x8D),
            pair(regex(R"(^\{\s+shift spc\s*\})"), 0xA0),

            // cursor
            pair(regex(R"(^\{\s+up\s*\})"), 0x91),
            pair(regex(R"(^\{\s+down\s*\})"), 0x11),
            pair(regex(R"(^\{\s+left\s*\})"), 0x9D),
            pair(regex(R"(^\{\s+right\s*\})"), 0x1D),

            // clear and home
            pair(regex(R"(^\{\s+clr\s*\})"), 0x93),
            pair(regex(R"(^\{\s+home\s*\})"), 0x13),

            // insert and delete
            pair(regex(R"(^\{\s+insr\s*\})"), 0x94),
            pair(regex(R"(^\{\s+del\s*\})"), 0x14),

            // escape and tab
            pair(regex(R"(^\{\s+esc\s*\})"), 0x1B),
            pair(regex(R"(^\{\s+tab\s*\})"), 0x09),

            // function keys
            pair(regex(R"(^\{\s+run\s*\})"), 0x83),
            pair(regex(R"(^\{\s+help\s*\})"), 0x84),
            pair(regex(R"(^\{\s+F1\s*\})"), 0x85),
            pair(regex(R"(^\{\s+F2\s*\})"), 0x89),
            pair(regex(R"(^\{\s+F3\s*\})"), 0x86),
            pair(regex(R"(^\{\s+F4\s*\})"), 0x8A),
            pair(regex(R"(^\{\s+F5\s*\})"), 0x87),
            pair(regex(R"(^\{\s+F6\s*\})"), 0x8B),
            pair(regex(R"(^\{\s+F7\s*\})"), 0x88),
            pair(regex(R"(^\{\s+F8\s*\})"), 0x8C),
            pair(regex(), 0x00)
    };
    return cbm_codes;
}


static pair<regex, char> * getCharCodesList() {

    static pair<regex, char> cbm_codes[] = {

            // misc cbm chars
            pair(regex(R"(^\{\s+lb\s*\})"), 0x5C),
            pair(regex(R"(^\{\s+^\s*\})"), 0x5E),
            pair(regex(R"(^\{\s+<-\s*\})"), 0x5F),
            pair(regex(R"(^\{\s+checker\s*\})"), 0xA6),
            pair(regex(R"(^\{\s+b border\s*\})"), 0xA4),
            pair(regex(R"(^\{\s+h check\s*\})"), 0xA8),
            pair(regex(R"(^\{\s+tl tri\s*\})"), 0xA9),
            pair(regex(R"(^\{\s+br border\s*\})"), 0xBA),
            pair(regex(R"(^\{\s+hbar\s*\})"), 0xC0),
            pair(regex(R"(^\{\s+[+]\s*\})"), 0xDB),
            pair(regex(R"(^\{\s+v check\s*\})"), 0xDC),
            pair(regex(R"(^\{\s+vbar\s*\})"), 0xDD),
            pair(regex(R"(^\{\s+pi\s*\})"), 0xDE),
            pair(regex(R"(^\{\s+tr tri\s*\})"), 0xDF),

            // C= A-Z
            pair(regex(R"(^\{\s+a\s*\})"), 0xB0),
            pair(regex(R"(^\{\s+b\s*\})"), 0xBF),
            pair(regex(R"(^\{\s+c\s*\})"), 0xBC),
            pair(regex(R"(^\{\s+d\s*\})"), 0xAC),
            pair(regex(R"(^\{\s+e\s*\})"), 0xB1),
            pair(regex(R"(^\{\s+f\s*\})"), 0xBB),
            pair(regex(R"(^\{\s+g\s*\})"), 0xA5),
            pair(regex(R"(^\{\s+h\s*\})"), 0xB4),
            pair(regex(R"(^\{\s+i\s*\})"), 0xA2),
            pair(regex(R"(^\{\s+j\s*\})"), 0xB5),
            pair(regex(R"(^\{\s+k\s*\})"), 0xA1),
            pair(regex(R"(^\{\s+l\s*\})"), 0xB6),
            pair(regex(R"(^\{\s+m\s*\})"), 0xA7),
            pair(regex(R"(^\{\s+n\s*\})"), 0xAA),
            pair(regex(R"(^\{\s+o\s*\})"), 0xB9),
            pair(regex(R"(^\{\s+p\s*\})"), 0xAF),
            pair(regex(R"(^\{\s+q\s*\})"), 0xAB),
            pair(regex(R"(^\{\s+r\s*\})"), 0xB2),
            pair(regex(R"(^\{\s+s\s*\})"), 0xAE),
            pair(regex(R"(^\{\s+t\s*\})"), 0xA3),
            pair(regex(R"(^\{\s+u\s*\})"), 0xB8),
            pair(regex(R"(^\{\s+v\s*\})"), 0xBE),
            pair(regex(R"(^\{\s+w\s*\})"), 0xB3),
            pair(regex(R"(^\{\s+x\s*\})"), 0xBD),
            pair(regex(R"(^\{\s+y\s*\})"), 0xB7),
            pair(regex(R"(^\{\s+z\s*\})"), 0xAD),
            pair(regex(), 0x00)
    };
    return cbm_codes;
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-infinite-loop"  // inspection does not recognize tie updates control_ch
static char evalCbmCodes(pair<regex, char> *codes, string &str, string::iterator &pos) {
    string sub_str = str.substr(pos - str.begin());
    regex rx;
    int control_ch;
    smatch results;
    tie(rx, control_ch) = *codes;
    while (control_ch != 0) {} {
        if (regex_search(sub_str, results, rx)) {
            pos += results.length();
            return static_cast<char>(control_ch);
        }
        codes++;
        tie(rx, control_ch) = *codes;
    }
    return static_cast<char>(0xFF);
}
#pragma clang diagnostic pop

static string evalAsciiString(Location loc, string &line_text, string &str, string::iterator &pos) {
    static auto octal_regx = regex("^[0-7]{3}");
    static auto hex_regx = regex("^[0-9a-fA-F]{2}");

    auto end_pos = str.end() - 1;
    string out_str;
    smatch results;

    while (pos != end_pos) {
        if (*pos == '\'') {
            pos++;
            loc.column += static_cast<int>(pos - str.begin());
            if (pos == end_pos) {
                throw CasmErrorException("String hsa dangling '\\' character.", loc, line_text);
            }
            string sub_str = str.substr(pos - str.begin());
            switch (*pos) {
                case '\'':
                    out_str += '\'';
                    break;
                case '"':
                    out_str += '"';
                    break;
                case '?':
                    out_str += '?';
                    break;
                case '\\':
                    out_str += '\\';
                    break;
                case 'a':
                    out_str += static_cast<char>(0x07);
                    break;
                case 'b':
                    out_str += static_cast<char>(0x08);
                    break;
                case 'f':
                    out_str += static_cast<char>(0x0C);
                    break;
                case 'n':
                    out_str += static_cast<char>(0x0A);
                    break;
                case 'r':
                    out_str += static_cast<char>(0x0D);
                    break;
                case 't':
                    out_str += static_cast<char>(0x09);
                    break;
                case 'v':
                    out_str += static_cast<char>(0x07);
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                    if (not regex_search(sub_str, results, octal_regx)) {
                        throw CasmErrorException("Bad octal string escape.", loc, line_text);
                    }
                    out_str += static_cast<char>(stoi(results.str(), nullptr, 8));
                    pos += 2;
                    break;
                case 'x':
                case 'X':
                    if (not regex_search(sub_str, results, hex_regx)) {
                        throw CasmErrorException("Bad octal string escape.", loc, line_text);
                    }
                    out_str += static_cast<char>(stoi(results.str(), nullptr, 16));
                    pos += 2;
                    break;
                default:
                    throw CasmErrorException("Unknown ASCII escape sequence.", loc, line_text);
            }
        }
        else {
            out_str += *pos;
        }
        pos++;
    }
    return out_str;
}

static string evalCbmString(Location loc, string &line_text, string &str, string::iterator &pos) {
    string result_str;

    while (pos != str.end() - 1) {
        if (*pos != '\\' and *pos != '{') {
            result_str += *pos;
            pos++;
            continue;
        }
        if (*pos == '\\') {
            pos++;
            if (pos != str.end() - 1) {
                auto ch = static_cast<unsigned char>(*pos);
                if (ch >= 0x61 and ch <= 0x7E) {
                    ch &= static_cast<unsigned char>(0x1F);
                }
                else if (ch == '0') {
                    ch = 0;
                }
                result_str += ch;
            }
            else {
                loc.column += static_cast<int>(pos - str.begin());
                throw CasmErrorException("Dangling '\\' escape character at end of string.",
                                         loc, line_text);
            }
        }
        else {
            char ch;
            ch = evalCbmCodes(getCharCodesList(), str, pos);
            if (ch == static_cast<char>(0xFF)) {
                ch = evalCbmCodes(getControlCodesList(), str, pos);
            }
            if (ch == static_cast<char>(0xFF)) {
                loc.column += static_cast<int>(pos - str.begin());
                throw CasmErrorException("Unknown CBM code.", loc, line_text);
            }
            result_str += ch;
        }
    }
    return result_str;
}

static string evalScreenString(Location loc, string &line_text, string &str, string::iterator &pos) {
    string result_str;

    unsigned char ch;
    while (pos != str.end() - 1) {
        if (*pos != '{') {
            ch = *pos;
            pos++;
        }
        else {
            ch = evalCbmCodes(getCharCodesList(), str, pos);
            if (ch == 0xFF) {
                loc.column += static_cast<int>(pos - str.begin());
                throw CasmErrorException("Unknown CBM code for screen codes.", loc, line_text);
            }
        }
        if ((ch >= 0x40 && ch <= 0x5F) || (ch >= 0xA0 && ch <= 0xBF)) {
            ch -= 0x40;
        }
        else if (ch >= 0x60 && ch <= 0x7F) {
            ch -= 0x20;
        }
        else if (ch >= 0xC0 && ch <= 0xFE) {
            ch -= 0x80;
        }
        else if (ch == 0xFF) {
            ch = 0x5E;
        }
        result_str += ch;
    }
    return result_str;
}



static void evaluateString(ExpressionItem &exp_item, string &line_text) {
    string treeString = exp_item.stringValue;

    enum class StringType {ascii, petscii, screen};
    enum class EndType {asciiz, neg_bit, length_byte, nothing};

    string result_str;
    StringType str_type = StringType::ascii;
    EndType end_type = EndType::nothing;
    auto it = treeString.begin();
    if (*it == 'p') {
        str_type = StringType::petscii;
    }
    else if (*it == 's') {
        str_type = StringType::screen;
    }
    else if (*it != '\'' && *it != '"') {
        it++;
        if (*it == 'z') {
            end_type = EndType::asciiz;
        }
        else if (*it == 'n') {
            end_type = EndType::neg_bit;
        }
        else if (*it == 'l') {
            end_type = EndType::length_byte;
        }
    }
    else {
        it++; // This is the 'a' String type
    }
    it++; // Move past the single or double quote delimiter.

    switch (str_type) {
        case StringType::ascii:
            result_str = evalAsciiString(exp_item.loc, line_text, treeString, it);
            break;
        case StringType::petscii:
            result_str = evalCbmString(exp_item.loc, line_text, treeString, it);
            break;
        case StringType::screen:
            result_str = evalScreenString(exp_item.loc, line_text, treeString, it);
            break;
    }

    int len = result_str.length();
    switch (end_type) {
        case EndType::asciiz:
            result_str += static_cast<char>(0x00);
            break;
        case EndType::length_byte:
            result_str.insert(0, 1, static_cast<char>(len));
            break;
        case EndType::neg_bit:
            if (len > 0) {
                auto last_ch = result_str.back();
                last_ch = static_cast<unsigned char>(last_ch) | static_cast<unsigned char>(0x80);
                result_str.back() = last_ch;
            }
            break;
        default:
            ;
    }
    exp_item.stringValue = result_str;
}
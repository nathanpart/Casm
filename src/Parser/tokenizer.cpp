//
// Created by nathan on 7/18/20.
//

#include <regex>

#include "tokenizer.h"
#include "token.h"
#include "errcode.h"

using namespace std;

// Forward declaration for evaluators
static int eval_numeric(string &work_line, short &type, std::string &str, int &length);
static int eval_string(string &work_line, short &type, std::string &str, int &length);
static int eval_arg(string &work_line, short &type, std::string &str, int &length);
static int eval_name(string &work_line, short &type, std::string &str, int &length);
static int eval_symbol(string &work_line, short &type, std::string &str, int &length);

using evaluator = int (*)(string &work_line, short &type, std::string &str, int &length);

evaluator evaluators[] = {eval_numeric, eval_string, eval_arg, eval_name, eval_symbol};


int tokenizer::getToken(short &type, std::string &str, std::string &line_str, int &start_col) {
    static regex ws_pattern("^\\s*");

    // Fetch another line from the stream if it is needed.
    int err;
    if (nextLine) {
        if ((err = fetchLine(type, str, line_str, start_col)) != 0)
            return err;
    }


    line_str = line;
    string work_str = line.substr(column);   // Get the remaining untokenized part of the line.

    smatch results;

    // Eat any leading white space
    if (regex_search(work_str, results, ws_pattern)) {
        column += results.length();
        work_str = line.substr(column);
    }
    start_col = column;

    // Check to see if we hit the end of current line also for comments
    if (work_str.empty() || work_str[0] == ';' || (work_str[0] == '*' && column == 0)) {
        nextLine = true;
        type = NEWLINE;
        str = work_str;
        return E_OK;
    }

    // Figure out what we got
    for (auto eval_func: evaluators) {
        int length = 0;
        err = eval_func(work_str, type, str, length);
        if (err != 0) {
            column += length; // If we are returning a token with E_OK, we need to move to char following token
            return err;
        }
    }

    // At this point we have something unexpected
    type = ERRORTOKEN;
    str = work_str;
    return E_TOKEN;
}

int tokenizer::fetchLine(short &type, std::string &str, std::string &line_str, int &start_col) {
    column = 0;
    if (!getline(in_stream, line)) {
        int err = in_stream.eof() ? E_OK : E_STREAM;
        type = static_cast<short>((err == E_OK) ? ENDMARKER : ERRORTOKEN);
        str = "";
        line_str = line;
        start_col = 0;
        file_end = true;
        return err;
    }
    nextLine = false;
    return 0;
}


// The evaluators - that look for a particular type of token

int eval_numeric(string &work_line, short &type, string &str, int &length) {
    static regex dec_pattern("^[1-9][0-9]*");
    static regex oct_pattern("^0[0-7]*");
    static regex bin_pattern("^%[01]+");
    static regex hex_pattern("^\\$[0-9a-fA-F]+");

    smatch results;
    if (not regex_search(work_line, results, dec_pattern))
        if (not regex_search(work_line, results, oct_pattern))
            if (not regex_search(work_line, results, bin_pattern))
                if (not regex_match(work_line, results, hex_pattern))
                    return 0;
    str = results.str();
    length = results.length();
    type = NUMBER;
    return E_OK;
}

int eval_string(string &work_line, short &type, std::string &str, int &length) {
    static regex str_pattern("^[aps]?[znl]?(['\"])", regex::icase);

    smatch results;
    if (not regex_search(work_line, results, str_pattern)) {
        return 0;
    }
    char delimiter = results.str().back();
    bool active_back_slash = false;

    // Hunt for the closing delimiter
    for (auto it = work_line.begin() + results.length(); it != work_line.end(); it++) {
        if (*it == '\\' && !active_back_slash) {
            active_back_slash = true;
            continue;
        }
        if (active_back_slash) {
            active_back_slash = false;
            continue;
        }
        if (*it == delimiter) {
            it++;        // Advance to position following closing delimiter so that length calc includes it
            length = it - work_line.begin();
            str = work_line.substr(0, length);
            type = STRING;
            return E_OK;
        }
    }
    type = STRING;
    str = work_line;
    return E_EOL;
}

int eval_arg(string &work_line, short &type, std::string &str, int &length) {
    static regex arg_pattern("^`[^`]*`");

    bool arg_started = work_line[0] == '`';
    smatch results;
    if (not regex_search(work_line, results, arg_pattern)) {
        if (arg_started) {
            type = ARG;
            str = work_line;
            return E_EOL;
        } else {
            return 0;
        }
    }
    str = results.str();
    length = results.length();
    type = ARG;
    return E_OK;
}

int eval_name(string &work_line, short &type, std::string &str, int &length) {
    static pair<int, regex> keyword_tokens[] = {
            { GROUP1_INST, regex("^(adc|and|cmp|eor|lda|ora|sbc|sta)", regex::icase)},
            { GROUP2_INST, regex("^(asl|dec|inc|lsr|rol|ror)", regex::icase)},
            { GROUP3_INST, regex("^(bcc|bcs|beq|bmi|bne|bpl|bra|brl|bvc|bvs|jsl|pea|per|trb|tsb)", regex::icase)},
            { GROUP4_INST, regex("^(bit|ldy)", regex::icase)},
            { GROUP5_INST, regex("^(cpx|cpy)", regex::icase)},
            { GROUP6_INST, regex("^(rep|sep)", regex::icase)},
            { GROUP7_INST, regex("^(mvn|mvp)", regex::icase)},
            { GROUP8_INST, regex("^jmp", regex::icase)},
            { GROUP9_INST, regex("^jml", regex::icase)},
            { GROUP10_INST, regex("^jsr", regex::icase)},
            { GROUP11_INST, regex("^ldx", regex::icase)},
            { GROUP12_INST, regex("^stx", regex::icase)},
            { GROUP13_INST, regex("^(sty|stz)", regex::icase)},
            { GROUP14_INST, regex("^(clc|cld|cli|clv|dex|dey|inx|iny|nop)", regex::icase)},
            { GROUP14_INST, regex("^(pha|phb|phd|phk|php|phx|phy|pla|plb|pld|plp|plx|ply)", regex::icase)},
            { GROUP14_INST, regex("^(rti|rtl|rts|sec|sed|sei|stp)", regex::icase)},
            { GROUP14_INST, regex("^(tax|tay|tcd|tcs|tdc|tsc|tsx|txa|txs|txy|tya|tyx)", regex::icase)},
            { GROUP14_INST, regex("^(tad|tas|tda|tsa)", regex::icase)},
            { GROUP14_INST, regex("^(wai|wdm|xba|swa|xce)", regex::icase)},
            { GROUP15_INST, regex("^pei", regex::icase)},
            { GROUP16_INST, regex("^(brk|cop)", regex::icase)},

            // R65C02 specific opcodes
            { GROUP7_INST, regex("^(bbr0|bbr1|bbr2|bbr3|bbr4|bbr5|bbr6|bbr7)", regex::icase)},
            { GROUP7_INST, regex("^(bbs0|bbs1|bbs2|bbs3|bbs4|bbs5|bbs6|bbs7)", regex::icase)},
            { GROUP15_INST, regex("^(rmb0|rmb1|rmb2|rmb3|rmb4|rmb5|rmb6|rmb7)", regex::icase)},
            { GROUP15_INST, regex("^(smb0|smb1|smb2|smb3|smb4|smb5|smb6|smb7)", regex::icase)},

            { EQU, regex("^equ", regex::icase)},
            { PARA, regex("^para", regex::icase)},
            { PAGE, regex("^page", regex::icase)},
            { BYTE, regex("^byte", regex::icase)},
            { WORD, regex("^word", regex::icase)},
            { DWORD, regex("^dword", regex::icase)},
            { ABS, regex("^abs", regex::icase)},
            { STORAGE, regex("^storage", regex::icase)},
            { SEGMENT, regex("^segment", regex::icase)},
            { ENDS, regex("^ends", regex::icase)},
            { MACRO, regex("^macro", regex::icase)},
            { ENDM, regex("^endm", regex::icase)},
            { WIDE, regex("^wide", regex::icase)},
            { SHORT, regex("^short", regex::icase)},
            { LONG, regex("^long", regex::icase)},
            { DP, regex("^dp", regex::icase)},
            { ZP, regex("^zp", regex::icase)},
            { AREG, regex("^a", regex::icase)},
            { XREG, regex("^x", regex::icase)},
            { YREG, regex("^y", regex::icase)},
            { SREG, regex("^s", regex::icase)},
            { CPU6502, regex("^cpu6502", regex::icase)},
            { CPU65C02, regex("^cpu65c02", regex::icase)},
            { CPU65C816, regex("^cpu65c816", regex::icase)},
            { WIDEA, regex("^widea", regex::icase)},
            { SHORTA, regex("^shorta", regex::icase)},
            { WIDEXY, regex("^widexy", regex::icase)},
            { SHORTXY, regex("^shortxy", regex::icase)},
            { END_BLOCK, regex("^endb", regex::icase)},
            { ELSE, regex("^else", regex::icase)},
            { ENDIF, regex("^endif", regex::icase)},
            { DB, regex("^db", regex::icase)},
            { DW, regex("^dw", regex::icase)},
            { DBW, regex("^dbw", regex::icase)},
            { DA, regex("^da", regex::icase)},
            { DS, regex("^ds", regex::icase)},
            { ALIGN, regex("^align", regex::icase)},
            { BLOCK, regex("^block", regex::icase)},
            { IFDEF, regex("^ifdef", regex::icase)},
            { IFNDEF, regex("^ifndef", regex::icase)},
            { IF, regex("^if", regex::icase)},
            { IMPORT, regex("^import", regex::icase)},
            { EXPORT, regex("^export", regex::icase)}
    };

    static regex name_pattern("^[a-zA-Z_][a-zA-Z0-9_]*");

    smatch results;
    if (not regex_search(work_line, results, name_pattern))
        return 0;

    length = results.length();
    str = results.str();

    for (const auto& token_item: keyword_tokens) {
        if (not regex_match(str, results, token_item.second))
            continue;
        type = token_item.first;
        return E_OK;
    }
    type = NAME;
    return E_OK;
}

int eval_symbol(string &work_line, short &type, std::string &str, int &length) {
    short token;
    if (work_line.length() > 1) {
        token = static_cast<short>(twoChars(work_line[0], work_line[1]));
        if (token != OP) {
            type = token;
            str = work_line.substr(0, 2);
            length = 2;
            return E_OK;
        }
    }

    token = static_cast<short>(oneChar(work_line[0]));
    if (token != OP) {
        type = token;
        str = work_line.substr(0, 1);
        length = 1;
        return E_OK;
    }
    return 0;
}



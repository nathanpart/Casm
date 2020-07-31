/* Auto-generated by Parser/pgen */

#include "token.h"

/* Token names */

const char * const casmTokenNames[] = {
    "ENDMARKER",
    "NAME",
    "NUMBER",
    "STRING",
    "NEWLINE",
    "EQU",
    "PARA",
    "PAGE",
    "SEGMENT",
    "ENDS",
    "ARG",
    "MACRO",
    "ENDM",
    "GROUP1_INST",
    "GROUP2_INST",
    "GROUP3_INST",
    "GROUP4_INST",
    "GROUP5_INST",
    "GROUP6_INST",
    "GROUP7_INST",
    "GROUP8_INST",
    "GROUP9_INST",
    "GROUP10_INST",
    "GROUP11_INST",
    "GROUP12_INST",
    "GROUP13_INST",
    "GROUP14_INST",
    "GROUP15_INST",
    "GROUP16_INST",
    "WIDE",
    "SHORT",
    "LONG",
    "DP",
    "ZP",
    "AREG",
    "XREG",
    "YREG",
    "SREG",
    "CPU6502",
    "CPU65C02",
    "CPU65C816",
    "WIDEA",
    "SHORTA",
    "WIDEXY",
    "SHORTXY",
    "END_BLOCK",
    "ELSE",
    "ENDIF",
    "DB",
    "DW",
    "DBW",
    "DA",
    "DS",
    "ALIGN",
    "BLOCK",
    "IFDEF",
    "IFNDEF",
    "IF",
    "QUESTION",
    "HASH",
    "LPAR",
    "RPAR",
    "LSQB",
    "RSQB",
    "COLON",
    "COMMA",
    "SEMI",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "VBAR",
    "AMPER",
    "LESS",
    "GREATER",
    "EQUAL",
    "DOT",
    "PERCENT",
    "LBRACE",
    "RBRACE",
    "EXCLAMATION",
    "EQEQUAL",
    "NOTEQUAL",
    "LESSEQUAL",
    "GREATEREQUAL",
    "TILDE",
    "CIRCUMFLEX",
    "LEFTSHIFT",
    "RIGHTSHIFT",
    "DOUBLEVBAR",
    "DUUBLEAMPER",
    "OP",
    "<ERRORTOKEN>",
    "<COMMENT>",
    "<NL>",
    "<ENCODING>",
    "<N_TOKENS>",
};

/* Return the token corresponding to a single character */

int
oneChar(int c1)
{
    switch (c1) {
    case '!': return EXCLAMATION;
    case '#': return HASH;
    case '%': return PERCENT;
    case '&': return AMPER;
    case '(': return LPAR;
    case ')': return RPAR;
    case '*': return STAR;
    case '+': return PLUS;
    case ',': return COMMA;
    case '-': return MINUS;
    case '.': return DOT;
    case '/': return SLASH;
    case ':': return COLON;
    case ';': return SEMI;
    case '<': return LESS;
    case '=': return EQUAL;
    case '>': return GREATER;
    case '?': return QUESTION;
    case '[': return LSQB;
    case ']': return RSQB;
    case '^': return CIRCUMFLEX;
    case '{': return LBRACE;
    case '|': return VBAR;
    case '}': return RBRACE;
    case '~': return TILDE;
    }
    return OP;
}

int
twoChars(int c1, int c2)
{
    switch (c1) {
    case '!':
        switch (c2) {
        case '=': return NOTEQUAL;
        }
        break;
    case '&':
        switch (c2) {
        case '&': return DUUBLEAMPER;
        }
        break;
    case '<':
        switch (c2) {
        case '<': return LEFTSHIFT;
        case '=': return LESSEQUAL;
        case '>': return NOTEQUAL;
        }
        break;
    case '=':
        switch (c2) {
        case '=': return EQEQUAL;
        }
        break;
    case '>':
        switch (c2) {
        case '=': return GREATEREQUAL;
        case '>': return RIGHTSHIFT;
        }
        break;
    case '|':
        switch (c2) {
        case '|': return DOUBLEVBAR;
        }
        break;
    }
    return OP;
}

int
threeChars(int c1, int c2, int c3)
{
    return OP;
}

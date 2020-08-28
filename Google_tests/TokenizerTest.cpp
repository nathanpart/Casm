#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by nathan on 8/26/20.
//

#include "gtest/gtest.h"
#include "../src/Parser/tokenizer.h"
#include "../src/Parser/token.h"
#include "../src/Parser/errcode.h"

using namespace std;

TEST(Tokenizer, Basic) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in("\n \n");
    tokenizer tok(text_in);

    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NEWLINE);
    EXPECT_EQ(r_code, E_OK);

    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NEWLINE);
    EXPECT_EQ(r_code, E_OK);

    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ENDMARKER);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, DecimalNumber) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in("34");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NUMBER);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, OctalNumber) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in("0348");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NUMBER);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), "034");
    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NUMBER);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), "8");
}

TEST(Tokenizer, BinaryNumber) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in("%011101");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NUMBER);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), "%011101");
}

TEST(Tokenizer, HexadecimalNumber) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in("$AF32");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NUMBER);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), "$AF32");
}

TEST(Tokenizer, DoubleQuoteString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"("String")");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"("String")");
}

TEST(Tokenizer, SingleQuoteString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"('String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"('String')");
}

TEST(Tokenizer, BadStr) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"('String)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_EOL);
}

TEST(Tokenizer, asciiString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(a'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(a'String')");
}

TEST(Tokenizer, asciiZeroString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(az'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(az'String')");
}

TEST(Tokenizer, asciiNegString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(an'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(an'String')");
}

TEST(Tokenizer, asciiLenString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(al'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(al'String')");
}

TEST(Tokenizer, petsciiString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(p'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(p'String')");
}

TEST(Tokenizer, petsciiZeroString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(pz'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(pz'String')");
}

TEST(Tokenizer, petsciiNegString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(pn'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(pn'String')");
}

TEST(Tokenizer, petsciiLenString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(pl'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(pl'String')");
}

TEST(Tokenizer, screenString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(s'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(s'String')");
}

TEST(Tokenizer, screenZeroString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(sz'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(sz'String')");
}

TEST(Tokenizer, screenNegString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(sn'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(sn'String')");
}

TEST(Tokenizer, screenLenString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(sl'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(sl'String')");
}


TEST(Tokenizer, icaseString) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(sL'String')");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STRING);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(sL'String')");
}

TEST(Tokenizer, MacroArgument) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(`String`)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ARG);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(`String`)");
}

TEST(Tokenizer, MacroArg2) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(`Str`ing`)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ARG);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(`Str`)");
}

TEST(Tokenizer, BadArg) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(`String)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ARG);
    EXPECT_EQ(r_code, E_EOL);
}


TEST(Tokenizer, Group1) {
    vector<string> instructions{"adc", "aNd", "cmp", "eor", "lda", "ora", "sbc", "sta"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP1_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group2) {
    vector<string> instructions{"asl", "dec", "inc", "lsr", "rol", "ror"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP2_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group3) {
    vector<string> instructions{"bcc", "bcs", "beq", "bmi", "bne", "bpl", "bra", "brl", "bvc", "bvs", "jsl", "pea",
                                "per", "trb", "tsb"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP3_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group4) {
    vector<string> instructions{"bit", "ldy"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP4_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group5) {
    vector<string> instructions{"cpx", "cpy"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP5_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group6) {
    vector<string> instructions{"rep", "sep"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP6_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group7) {
    vector<string> instructions{"mvn", "mvp",
                                "bbr0", "bbr1", "bbr2", "bbr3", "bbr4", "bbr5", "bbr6", "bbr7",
                                "bbs0", "bbs1", "bbs2", "bbs3", "bbs4", "bbs5", "bbs6", "bbs7"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP7_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group8) {
    vector<string> instructions{"jmp"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP8_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group9) {
    vector<string> instructions{"jml"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP9_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group10) {
    vector<string> instructions{"jsr"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP10_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group11) {
    vector<string> instructions{"ldx"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP11_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group12) {
    vector<string> instructions{"stx"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP12_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group13) {
    vector<string> instructions{"sty", "stz"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP13_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group14) {
    vector<string> instructions{"clc", "cld", "clv", "dex", "dey", "inx", "iny", "nop",
                                "pha", "phb", "phd", "phk", "php", "phx", "phy",
                                "pla", "plb", "pld", "plp", "plx", "ply",
                                "rti", "rtl", "rts", "sec", "sed", "sei", "stp",
                                "tax", "tay", "tcd", "tcs", "tdc", "tsc", "tsx", "txa", "txs", "txy", "tya", "tyx",
                                "tad", "tas", "tda", "tsa", "wai", "wdm", "xba", "swa", "xce"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP14_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group15) {
    vector<string> instructions{"pei",
                                "rmb0", "rmb1", "rmb2", "rmb3", "rmb4", "rmb5", "rmb6", "rmb7",
                                "smb0", "smb1", "smb2", "smb3", "smb4", "smb5", "smb6", "smb7"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP15_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Group16) {
    vector<string> instructions{"brk", "cop"};
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    for (const auto& instr: instructions) {
        istringstream text_in(instr);
        tokenizer tok(text_in);
        int r_code = tok.getToken(type, str, line, col);
        EXPECT_EQ(type, GROUP16_INST);
        EXPECT_EQ(r_code, E_OK);
        EXPECT_STREQ(str.c_str(), instr.c_str());
    }
}

TEST(Tokenizer, Equ) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(equ)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, EQU);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(equ)");
}

TEST(Tokenizer, Para) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(para)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, PARA);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(para)");
}

TEST(Tokenizer, Page) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(page)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, PAGE);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(page)");
}

TEST(Tokenizer, Byte) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(byte)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, BYTE);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(byte)");
}

TEST(Tokenizer, Word) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(word)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, WORD);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(word)");
}

TEST(Tokenizer, Dword) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(dword)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DWORD);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(dword)");
}

TEST(Tokenizer, Abs) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(abs)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ABS);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(abs)");
}

TEST(Tokenizer, Storage) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(storage)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, STORAGE);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(storage)");
}

TEST(Tokenizer, Segment) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(segment)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, SEGMENT);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(segment)");
}

TEST(Tokenizer, Ends) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(ends)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ENDS);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(ends)");
}

TEST(Tokenizer, Macro) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(macro)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, MACRO);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(macro)");
}

TEST(Tokenizer, Endm) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(endm)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ENDM);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(endm)");
}

TEST(Tokenizer, Wide) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(wide)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, WIDE);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(wide)");
}

TEST(Tokenizer, Short) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(short)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, SHORT);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(short)");
}

TEST(Tokenizer, Long) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(long)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, LONG);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(long)");
}

TEST(Tokenizer, Dp) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(dp)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DP);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(dp)");
}

TEST(Tokenizer, Zp) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(zp)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ZP);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(zp)");
}

TEST(Tokenizer, A) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(a)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, AREG);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(a)");
}

TEST(Tokenizer, X) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(x)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, XREG);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(x)");
}

TEST(Tokenizer, Y) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(y)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, YREG);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(y)");
}

TEST(Tokenizer, S) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(s)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, SREG);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(s)");
}

TEST(Tokenizer, cpu6502) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(cpu6502)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, CPU6502);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(cpu6502)");
}

TEST(Tokenizer, cpu65c02) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(cpu65c02)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, CPU65C02);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(cpu65c02)");
}

TEST(Tokenizer, cpu65c816) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(cpu65c816)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, CPU65C816);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(cpu65c816)");
}

TEST(Tokenizer, WideA) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(widea)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, WIDEA);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(widea)");
}

TEST(Tokenizer, ShortA) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(shorta)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, SHORTA);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(shorta)");
}

TEST(Tokenizer, WideXY) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(widexy)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, WIDEXY);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(widexy)");
}

TEST(Tokenizer, ShortXY) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(shortxy)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, SHORTXY);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(shortxy)");
}

TEST(Tokenizer, Endb) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(endb)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, END_BLOCK);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(endb)");
}

TEST(Tokenizer, Else) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(else)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ELSE);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(else)");
}

TEST(Tokenizer, EndIf) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(endif)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ENDIF);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(endif)");
}

TEST(Tokenizer, Db) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(db)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DB);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(db)");
}

TEST(Tokenizer, Dw) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(dw)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DW);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(dw)");
}

TEST(Tokenizer, Dbw) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(dbw)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DBW);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(dbw)");
}

TEST(Tokenizer, Da) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(da)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DA);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(da)");
}

TEST(Tokenizer, Ds) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(ds)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DS);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(ds)");
}

TEST(Tokenizer, Align) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(align)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, ALIGN);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(align)");
}


TEST(Tokenizer, Block) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(block)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, BLOCK);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(block)");
}

TEST(Tokenizer, IfDef) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(ifdef)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, IFDEF);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(ifdef)");
}

TEST(Tokenizer, IfNDef) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(ifndef)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, IFNDEF);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(ifndef)");
}

TEST(Tokenizer, If) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(if)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, IF);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(if)");
}

TEST(Tokenizer, Import) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(import)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, IMPORT);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(import)");
}

TEST(Tokenizer, Export) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(export)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, EXPORT);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(export)");
}

TEST(Tokenizer, Name) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(Anna)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NAME);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(Anna)");
}

TEST(Tokenizer, NameUnderscore) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(_Anna_R_N)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NAME);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(_Anna_R_N)");
}

TEST(Tokenizer, MameWithNumbers) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(Number123)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NAME);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_STREQ(str.c_str(), R"(Number123)");
}

TEST(Tokenizer, NotEqual) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(!=)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NOTEQUAL);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, DoubleAmper) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(&&)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DUUBLEAMPER);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, LeftShift) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(<<)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, LEFTSHIFT);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, LessEqual) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(<=)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, LESSEQUAL);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, NotEqual2) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(<>)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NOTEQUAL);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, EqEqual) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(==)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, EQEQUAL);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, GreaterEqual) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(>=)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, GREATEREQUAL);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, RightShift) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(>>)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, RIGHTSHIFT);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, DoubleVBar) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(||)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DOUBLEVBAR);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Exclamation) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(!)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, EXCLAMATION);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Hash) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(#)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, HASH);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Percent) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(%)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, PERCENT);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Amper) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(&)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, AMPER);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, LPar) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(()");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, LPAR);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, RPar) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"())");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, RPAR);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Star) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(*)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NEWLINE);
    EXPECT_EQ(r_code, E_OK);

    text_in = istringstream{R"( *)"};
    tokenizer t{text_in};
    r_code = t.getToken(type, str, line, col);
    EXPECT_EQ(type, STAR);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Plus) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(+)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, PLUS);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Comma) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(,)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, COMMA);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Minus) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(-)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, MINUS);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Dot) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(.)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DOT);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Slash) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(/)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, SLASH);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Colon) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(:)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, COLON);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Semi) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(;)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, NEWLINE);   // Semi-colon trips the comment ignoring and thus SEMI will never be generated
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Less) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(<)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, LESS);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Equal) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(=)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, EQUAL);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Greater) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(>)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, GREATER);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Question) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(?)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, QUESTION);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, LSqb) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"([)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, LSQB);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, RSqb) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(])");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, RSQB);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Circumflex) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(^)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, CIRCUMFLEX);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, LBrace) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"({)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, LBRACE);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, VBar) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(|)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, VBAR);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, RBrace) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(})");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, RBRACE);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Tilde) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(~)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, TILDE);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Column) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(~ >>> .)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, TILDE);
    EXPECT_EQ(r_code, E_OK);
    EXPECT_EQ(col, 0);

    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, RIGHTSHIFT);
    EXPECT_EQ(col, 2);

    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, GREATER);
    EXPECT_EQ(col, 4);

    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DOT);
    EXPECT_EQ(col, 6);
}

TEST(Tokenizer, EndLine) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in("~ >>>\n .");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, TILDE);
    EXPECT_EQ(r_code, E_OK);

    tok.endLine();
    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, DOT);
    EXPECT_EQ(r_code, E_OK);
}

TEST(Tokenizer, Unknown) {
    short type{ 0 };
    string str;
    string line;
    int col{ 1 };
    istringstream text_in(R"(~ \)");
    tokenizer tok(text_in);
    int r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(type, TILDE);
    EXPECT_EQ(r_code, E_OK);

    r_code = tok.getToken(type, str, line, col);
    EXPECT_EQ(r_code, E_TOKEN);
}

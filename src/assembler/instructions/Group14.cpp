//
// Created by nathan on 7/30/20.
//

#include <string>
#include <regex>

#include "Group14.h"
#include "../../Parser/graminit.h"
#include "../Line.h"
#include "../Error.h"

using namespace std;

static const int clc = 0x18; static const int cld = 0xD8;
static const int cli = 0x58; static const int clv = 0xB8;
static const int dex = 0xCA; static const int dey = 0x88;
static const int inx = 0xE8; static const int iny = 0xC8;
static const int nop = 0xEA; static const int pha = 0x48;
static const int phb = 0x8B; static const int phd = 0x0B;
static const int phk = 0x4B; static const int php = 0x08;
static const int phx = 0xDA; static const int phy = 0x5A;
static const int pla = 0x68; static const int plb = 0xAB;
static const int pld = 0x2B; static const int plp = 0x28;
static const int plx = 0xFA; static const int ply = 0x7A;
static const int rti = 0x40; static const int rtl = 0x6B;
static const int rts = 0x60; static const int sec = 0x38;
static const int sed = 0xF8; static const int sei = 0x78;
static const int stp = 0xDB; static const int tax = 0xAA;
static const int tay = 0xA8; static const int tcd = 0x5B;
static const int tas = 0x1B; static const int tcs = 0x1B;
static const int tdc = 0x7B; static const int tsc = 0x3B;
static const int tsx = 0xBA; static const int txa = 0x8A;
static const int txs = 0x9A; static const int txy = 0x9B;
static const int tya = 0x98; static const int tyx = 0xBB;
static const int wai = 0xCB; static const int wdm = 0x42;
static const int xba = 0xEB; static const int xce = 0xFB;


void Group14::createInstruction(node &group_node, Line &asm_line) {
    static pair<regex, int> implied_op_codes[] = {
            { regex("clc", regex::icase), clc }, { regex("cld", regex::icase), cld },
            { regex("cli", regex::icase), cli }, { regex("clv", regex::icase), clv },
            { regex("dex", regex::icase), dex }, { regex("dey", regex::icase), dey },
            { regex("inx", regex::icase), inx }, { regex("iny", regex::icase), iny },
            { regex("nop", regex::icase), nop }, { regex("pha", regex::icase), pha },
            { regex("phb", regex::icase), phb }, { regex("phd", regex::icase), phd },
            { regex("phk", regex::icase), phk }, { regex("php", regex::icase), php },
            { regex("phx", regex::icase), phx }, { regex("phy", regex::icase), phy },
            { regex("pla", regex::icase), pla }, { regex("plb", regex::icase), plb },
            { regex("pld", regex::icase), pld }, { regex("plp", regex::icase), plp },
            { regex("plx", regex::icase), plx }, { regex("ply", regex::icase), ply },
            { regex("rti", regex::icase), rti }, { regex("rtl", regex::icase), rtl },
            { regex("rts", regex::icase), rts }, { regex("sec", regex::icase), sec },
            { regex("sed", regex::icase), sed }, { regex("sei", regex::icase), sei },
            { regex("stp", regex::icase), stp }, { regex("tax", regex::icase), tax },
            { regex("tay", regex::icase), tay }, { regex("tcd", regex::icase), tcd },
            { regex("tad", regex::icase), tcd }, { regex("tas", regex::icase), tas },
            { regex("tcs", regex::icase), tcs }, { regex("tdc", regex::icase), tdc },
            { regex("tda", regex::icase), tdc }, { regex("tsa", regex::icase), tsc },
            { regex("tsc", regex::icase), tsc }, { regex("tsx", regex::icase), tsx },
            { regex("txa", regex::icase), txa }, { regex("txs", regex::icase), txs },
            { regex("txy", regex::icase), txy }, { regex("tya", regex::icase), tya },
            { regex("tyx", regex::icase), tyx }, { regex("wai", regex::icase), wai },
            { regex("wdm", regex::icase), wdm }, { regex("xba", regex::icase), xba },
            { regex("xce", regex::icase), xce }, { regex("swa", regex::icase), xba }
    };

    const auto& child_node = group_node.child[0];
    smatch results;
    for (const auto& item: implied_op_codes) {
        if (regex_match(child_node.str, results, item.first)) {
            asm_line.instruction = unique_ptr<Instruction>(new Group14(item.second));
            break;
        }
    }
    asm_line.addressMode = AddressModes::imp;
}

void Group14::pass1(Line &asm_line, AsmState &state) {
    switch (opCode) {
        case phb: case phd: case phk: case plb:
        case pld: case rtl: case stp: case tcd:
        case tcs: case tdc: case tsc: case txy:
        case tyx: case wai: case wdm: case xba:
        case xce:
            if (state.cpuType != CpuType::CPU_65C816) {
                throw CasmErrorException("Instruction only available with 65C816",
                                         asm_line.instructionLoc, asm_line.lineText);
            }
            break;

        case phx: case phy: case plx: case ply:
            if (state.cpuType == CpuType::CPU_6502) {
                throw CasmErrorException("Instruction not available on a 6502.",
                                         asm_line.instructionLoc, asm_line.lineText);
            }
            break;

    }
    Instruction::pass1(asm_line, state);
}


//
// Created by nathan on 7/30/20.
//

#include <string>
#include <regex>

#include "Group14.h"
#include "../../Parser/graminit.h"
#include "../Line.h"

using namespace std;

void Group14::createInstruction(node &group_node, Line &asm_line) {
    static pair<regex, int> implied_op_codes[] = {
            { regex("clc", regex::icase), 0x18 }, { regex("cld", regex::icase), 0xD8 },
            { regex("cli", regex::icase), 0x58 }, { regex("clv", regex::icase), 0xB8 },
            { regex("dex", regex::icase), 0xCA }, { regex("dey", regex::icase), 0x88 },
            { regex("inx", regex::icase), 0xE8 }, { regex("iny", regex::icase), 0xC8 },
            { regex("nop", regex::icase), 0xEA }, { regex("pha", regex::icase), 0x48 },
            { regex("phb", regex::icase), 0x8B }, { regex("phd", regex::icase), 0x0B },
            { regex("phk", regex::icase), 0x4B }, { regex("php", regex::icase), 0x08 },
            { regex("phx", regex::icase), 0xDA }, { regex("phy", regex::icase), 0x5A },
            { regex("pla", regex::icase), 0x68 }, { regex("plb", regex::icase), 0xAB },
            { regex("pld", regex::icase), 0x2B }, { regex("plp", regex::icase), 0x28 },
            { regex("plx", regex::icase), 0xFA }, { regex("ply", regex::icase), 0x7A },
            { regex("rti", regex::icase), 0x40 }, { regex("rtl", regex::icase), 0x6B },
            { regex("rts", regex::icase), 0x60 }, { regex("sec", regex::icase), 0x38 },
            { regex("sed", regex::icase), 0xF8 }, { regex("sei", regex::icase), 0x78 },
            { regex("stp", regex::icase), 0xDB }, { regex("tax", regex::icase), 0xAA },
            { regex("tay", regex::icase), 0xA8 }, { regex("tcd", regex::icase), 0x5B },
            { regex("tad", regex::icase), 0x5B }, { regex("tas", regex::icase), 0x1B },
            { regex("tcs", regex::icase), 0x1B }, { regex("tdc", regex::icase), 0x7B },
            { regex("tda", regex::icase), 0x7B }, { regex("tsa", regex::icase), 0x3B },
            { regex("tsc", regex::icase), 0x3B }, { regex("tsx", regex::icase), 0xBA },
            { regex("txa", regex::icase), 0x8A }, { regex("txs", regex::icase), 0x9A },
            { regex("txy", regex::icase), 0x9B }, { regex("tya", regex::icase), 0x98 },
            { regex("tyx", regex::icase), 0xBB }, { regex("wai", regex::icase), 0xCB },
            { regex("wdm", regex::icase), 0x42 }, { regex("xba", regex::icase), 0xEB },
            { regex("xce", regex::icase), 0xFB }, { regex("swa", regex::icase), 0xEB }
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


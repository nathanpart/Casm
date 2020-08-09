//
// Created by nathan on 8/3/20.
//

#include <memory>
#include <string>

#include "Region.h"
#include "../Parser/graminit.h"
#include "../Parser/token.h"
#include "Line.h"
#include "Error.h"
#include "Segment.h"

using namespace std;

void Region::createInstruction(node &inst_node, Line &asm_line){
    auto *region_line = new Region();
    for (const auto& region_item: inst_node.child) {
        Expression expression;
        switch (region_item.type) {
            case SEGMENT:
                if (!asm_line.normalLabel()) {
                    throw CasmErrorException("SEGMENT missing name label.",
                                             region_item.location, asm_line.lineText);
                }
                region_line->segmentName = asm_line.label;
                asm_line.instructionLoc = region_item.location;
                break;
            case BYTE:
                region_line->alignment = AlignType::byte;
                break;
            case WORD:
                region_line->alignment = AlignType::word;
                break;
            case DWORD:
                region_line->alignment = AlignType::dword;
                break;
            case PARA:
                region_line->alignment = AlignType::para;
                break;
            case PAGE:
                region_line->alignment = AlignType::page;
                break;
            case ABS:
                region_line->segmentType = SegmentType::absolute;
                break;
            case DP:
                region_line->segmentType = SegmentType::direct_page;
                break;
            case ZP:
                region_line->segmentType = SegmentType::zero_page;
                break;
            case STORAGE:
                region_line->segmentType = SegmentType::storage;
                break;
            case exp:
                expression.buildRpnList(region_item, asm_line.lineText);
                asm_line.expressionList.push_back({region_item.location, expression});
                break;
            case ENDS:
                region_line->isEndSegment = true;
                asm_line.instructionLoc = region_item.location;
                break;
        }
    }
    asm_line.instruction = unique_ptr<Instruction>(region_line);
}

void Region::pass1(Line &asm_line, AsmState &state) {
    if (isEndSegment) {
        if (!state.inSegment()) {
            throw CasmErrorException("Not currently is a segment.", asm_line.instructionLoc, asm_line.lineText);
        }
        state.endSegment();
        return;
    }
    state.enterSegment(segmentName, segmentType, alignment);
}

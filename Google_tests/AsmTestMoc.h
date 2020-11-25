//
// Created by nathan on 10/26/20.
//

#ifndef CASM_ASMTESTMOC_H
#define CASM_ASMTESTMOC_H

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../assembler/AsmState.h"

class MockAsmState : public AsmStateInterface {
public:
    MOCK_METHOD(bool, resolveSymbol, (const std::string& symbol_name, Value &value, Location &loc), (override));
    MOCK_METHOD(int, getCurrentOffset, (Location &loc), (override));
    MOCK_METHOD(void, pass2Setup, (), (override));
    MOCK_METHOD(bool, inSegment, (),  (override));
    MOCK_METHOD(void, endSegment, (), (override));
    MOCK_METHOD(bool, isActive, (), (const, override));
    MOCK_METHOD(bool, inSegment, (), (const, override));
    MOCK_METHOD(Segment *, getCurrentSegment, (), (override));
    MOCK_METHOD(void, enterSegment, (std::string &name, SegmentType &seg_type, AlignType &align_type), (override));
    MOCK_METHOD(void, assignSymbol, (std::string &name, Value &value), (override));
    MOCK_METHOD(void, doAlignment, (AlignType &align_type, Location &loc), (override));
    MOCK_METHOD(void, enterBlock, (int block_address), (override));
    MOCK_METHOD(bool, inBlock, (), (override));
    MOCK_METHOD(void, endBlock, (), (override));
    MOCK_METHOD(void, allocateSpace, (int size), (override));
    MOCK_METHOD(void, startConditionalBlock, (bool state), (override));
    MOCK_METHOD(void, flipConditionalState, (), (override));
    MOCK_METHOD(void, endConditionalBlock, (), (override));
    MOCK_METHOD(bool, getActiveFlag, (), (const, override));
    MOCK_METHOD(bool, hasSymbol,(const std::string& symbol_name), (override));
    MOCK_METHOD(void, storeByte, (uint8_t byt), (override));
    MOCK_METHOD(void, addRelocationEntry, (const Value& value, int operand_size, const Location& loc), (override));
    MOCK_METHOD((std::map<std::string, Segment> &), getSegments, (), (override));
    MOCK_METHOD(void, importSymbol, (const std::string& local_name, std::string& symbol_name, std::string& seg_name), (override));
    MOCK_METHOD(void, exportSymbol, (const std::string& label_name, const std::string& extern_name), (override));
    MOCK_METHOD(void, incrGlobalLineNumber, (), (override));
    MOCK_METHOD(std::string, getCurrentSegName, (), (override));
    MOCK_METHOD(Line *, getCurrentLine, (), (override));

};

#endif //CASM_ASMTESTMOC_H

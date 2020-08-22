//
// Created by nathan on 8/21/20.
//

#ifndef CASM_OBJECTFILE_H
#define CASM_OBJECTFILE_H

#include <string>
#include <vector>
#include <cstdio>
#include <boost/endian/arithmetic.hpp>
#include <boost/static_assert.hpp>

struct FileHeader {
    boost::endian::little_uint8_t fileId[8];
    boost::endian::little_uint32_t stringTable;
    boost::endian::little_uint32_t segmentCount;
};

struct SegmentHeader {
    boost::endian::little_uint32_t segmentNameStr;
    boost::endian::little_uint16_t baseAddress;
    boost::endian::little_uint8_t type;
    boost::endian::little_uint8_t alignment;
    boost::endian::little_uint32_t exportCount;
    boost::endian::little_uint32_t exportTable;
    boost::endian::little_uint32_t importCount;
    boost::endian::little_uint32_t importReferences;
    boost::endian::little_uint32_t sectionCount;
    boost::endian::little_uint32_t sections;
};

struct SectionHeader {
    boost::endian::little_uint16_t startAddress;
    boost::endian::little_uint16_t size;
    boost::endian::little_uint8_t alignment;
    boost::endian::little_uint8_t blockFlag;
    boost::endian::little_uint32_t objectCode;
    boost::endian::little_uint32_t relocationCount;
    boost::endian::little_uint32_t relocationTable;
    boost::endian::little_uint32_t segmentRefCount;
    boost::endian::little_uint32_t segmentRefs;
};

struct RelocationEntry {
    boost::endian::little_uint16_t offset;
    boost::endian::little_uint16_t byteBaseOffset;
    boost::endian::little_uint8_t type;
};

struct ExportEntry {
    boost::endian::little_uint32_t name;
    boost::endian::little_uint32_t segmentName;
    boost::endian::little_uint16_t offset;
};

struct ImportEntry {
    boost::endian::little_uint32_t name;
    boost::endian::little_uint32_t segmentName;
    boost::endian::little_uint16_t offset;
    boost::endian::little_uint8_t type;
};

struct StringEntry {
    boost::endian::little_uint16_t strLength;
    std::string s;
};

class StringTable {
    unsigned index = 0;
    std::vector<StringEntry> stringList;
public:
    unsigned addString(const std::string& str);
    bool write(FILE *fptr);
};

class AsmState;

bool writeObjectFile(const std::string &file_name, AsmState &state);




#endif //CASM_OBJECTFILE_H

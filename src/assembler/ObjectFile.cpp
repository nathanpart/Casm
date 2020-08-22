//
// Created by nathan on 8/21/20.
//

#include <string>
#include <cstdio>
#include <boost/endian/arithmetic.hpp>

#include "ObjectFile.h"
#include "AsmState.h"
#include "Segment.h"

using namespace boost::endian;
using namespace std;


unsigned StringTable::addString(const std::string& str) {
    StringEntry entry;
    entry.strLength = str.length();
    entry.s = str;
    stringList.push_back(entry);
    return index++;
}

bool StringTable::write(FILE *fptr) {
    for (auto &string_entry: stringList) {
        if (fwrite(&string_entry.strLength, sizeof(string_entry.strLength), 1, fptr) != 1) {
            return false;
        }
        if (fwrite(string_entry.s.data(), string_entry.strLength, 1, fptr) != 1) {
            return false;
        }
    }
    return true;
}


bool writeObjectFile(const string &file_name, AsmState &state) {
    unsigned object_offset = 0;
    auto string_table = StringTable();
    FILE *obj_file = fopen(file_name.c_str(), "w+b");
    if (!obj_file) {
        return false;
    }
    map<string, Segment> &segment_list = state.getSegments();

    // Write the initial file header
    auto file_header = FileHeader();
    string id_str = "CASM.OBJ";
    for (int i = 0; i < 8; i++) {
        file_header.fileId[i] = id_str[i];
    }
    file_header.segmentCount = segment_list.size();
    if (fwrite(&file_header, sizeof(FileHeader), 1, obj_file) != 1) {
        return false;
    }

    long segments_position = ftell(obj_file);
    if (segments_position == -1L) {
        return false;
    }

    // Write out the initial segment headers
    for (auto &segment: segment_list) {
        auto seg_header = segment.second.getSegmentHeader(string_table);
        if (fwrite(&seg_header, sizeof(SegmentHeader), 1, obj_file) != 1) {
            return false;
        }
    }

    long obj_offset;

    // Write out the tables in each segment
    long current_segment = 0;
    for (auto &segment: segment_list) {
        long sections_position = ftell(obj_file);
        if (sections_position == -1L) {
            return false;
        }

        // Read the segment header of the segment having its tables written out
        SegmentHeader seg_header{};
        if (fseek(obj_file, segments_position + current_segment *
                    static_cast<long>(sizeof(SegmentHeader)), SEEK_SET) != 0) {
            return false;
        }
        if (fread(&seg_header, sizeof(SegmentHeader), 1, obj_file) != 1) {
            return false;
        }

        // Write out the section headers for the segment
        seg_header.sections = sections_position;
        if (fseek(obj_file, sections_position, SEEK_SET) != 0) {
            return false;
        }
        SectionList &sections = segment.second.getSections();
        auto section_header = SectionHeader();
        for (auto &section: sections) {
            section_header = {};
            section_header.startAddress = section.startAddress;
            section_header.size = section.size;
            section_header.alignment = static_cast<int>(section.alignment);
            section_header.blockFlag = section.isBlock ? 0xFFu : 0x00u;
            section_header.relocationCount = section.relocationTable.size();
            section_header.segmentRefCount = section.segmentRefs.size();

            if (fwrite(&section_header, sizeof(SectionHeader), 1, obj_file) != 1) {
                return false;
            }
        }

        // Write out the tables in the sections
        long cur_section = 0;
        for (auto &section: sections) {
            obj_offset = ftell(obj_file);
            if (obj_offset == -1L) {
                return false;
            }

            // Get section header for the section that is having its tables written out
            if (fseek(obj_file,
                      sections_position + cur_section * static_cast<long>(sizeof(SectionHeader)), SEEK_SET) != 0) {
                return false;
            }
            if (fread(&section_header, sizeof(SectionHeader), 1, obj_file) != 1) {
                return false;
            }

            // Write out the object code
            section_header.objectCode = obj_offset;
            if (fseek(obj_file, obj_offset, SEEK_SET) != 0) {
                return false;
            }
            if (fwrite(section.objectCode.data(), section.objectCode.size(), 1, obj_file) != 1) {
                return false;
            }

            // write out the section's relocation table
            obj_offset = ftell(obj_file);
            if (obj_offset == -1L) {
                return false;
            }
            section_header.relocationTable = object_offset;
            for (auto &relocation_entry: section.relocationTable) {
                auto rel_rec = RelocationEntry();
                rel_rec.offset = relocation_entry.offset;
                rel_rec.byteBaseOffset = relocation_entry.baseOffset;
                rel_rec.type = static_cast<int>(relocation_entry.type);
                if (fwrite(&rel_rec, sizeof(RelocationEntry), 1, obj_file) != 1) {
                    return false;
                }
            }

            // write out the section's segment references
            obj_offset = ftell(obj_file);
            if (obj_offset == -1L) {
                return false;
            }
            section_header.segmentRefs = object_offset;
            for (auto &seg_item: section.segmentRefs) {
                little_uint16_t seg_ref = seg_item;
                if (fwrite(&seg_ref, sizeof(little_uint16_t), 1, obj_file) != 1) {
                    return false;
                }
            }

            // Save the updated section header
            obj_offset = ftell(obj_file);
            if (obj_offset == -1L) {
                return false;
            }
            if (fseek(obj_file,
                      sections_position + cur_section * static_cast<long>(sizeof(SectionHeader)), SEEK_SET) != 0) {
                return false;
            }
            if (fwrite(&section_header, sizeof(SectionHeader), 1, obj_file) != 1) {
                return false;
            }
            if (fseek(obj_file, obj_offset, SEEK_SET) !=0) {
                return false;
            }

            cur_section++;
        }

        // Write out the segment's export tables
        if (fseek(obj_file, obj_offset, SEEK_SET) != 0)  {
            return false;
        }
        seg_header.exportTable = obj_offset;
        for (auto &export_item: segment.second.getExports()) {
            auto export_entry = ExportEntry();
            export_entry.name = string_table.addString(export_item.second.name);
            export_entry.segmentName = string_table.addString(export_item.second.segmentName);
            export_entry.offset = export_item.second.location;
            if (fwrite(&export_entry, sizeof(ExportEntry), 1, obj_file) != 1) {
                return false;
            }
        }

        // Write out the segment's import tables
        obj_offset = ftell(obj_file);
        if (obj_offset == -1L) {
            return false;
        }
        seg_header.importReferences = obj_offset;
        for (auto &import_item: segment.second.getImportRefs()) {
            auto import_entry = ImportEntry();
            import_entry.name = string_table.addString(import_item.name);
            import_entry.segmentName = string_table.addString(import_item.segmentName);
            import_entry.offset = import_item.location;
            import_entry.type = static_cast<int>(import_item.type);
            if (fwrite(&import_entry, sizeof(ImportEntry), 1, obj_file) != 1) {
                return false;
            }
        }

        // Write out the updated segment header
        obj_offset = ftell(obj_file);
        if (obj_offset == -1L) {
            return false;
        }
        if (fseek(obj_file, segments_position + current_segment *
                                                static_cast<long>(sizeof(SegmentHeader)), SEEK_SET) != 0) {
            return false;
        }
        if (fwrite(&seg_header, sizeof(SegmentHeader), 1, obj_file) != 1) {
            return false;
        }
        if (fseek(obj_file, obj_offset, SEEK_SET) != 0) {
            return false;
        }

        current_segment++;
    }

    // Write out the string table
    obj_offset = ftell(obj_file);
    if (obj_offset == -1L) {
        return false;
    }
    string_table.write(obj_file);
    if (fseek(obj_file, 0, SEEK_SET) != 0) {
        return false;
    }
    if (fread(&file_header, sizeof(FileHeader), 1, obj_file) != 1) {
        return false;
    }
    file_header.stringTable = obj_offset;
    if (fwrite(&file_header, sizeof(FileHeader), 1, obj_file) != 1) {
        return false;
    }
    fclose(obj_file);
    return true;
}


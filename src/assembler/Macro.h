//
// Created by nathan on 8/4/20.
//

#ifndef CASM_MACRO_H
#define CASM_MACRO_H

#include <iosfwd>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include "Location.h"

class SourceItem;

class Macro {
    static std::regex endm_pat;
    static std::regex mac;
    static std::regex expand_pat;
    static std::regex arg_pat;
    static int expansion_count;  // counter to catch runaway recursive macro expansion

    int level;
    std::string macro_name;
    Macro *ancestor;
    Location location;
    Location startLoc;
    std::map<std::string, Macro> nestedMacros;
    std::vector<std::string> macroLines;

    void addNested(SourceItem &source, const std::string& root_name);
    void doExpandMacro(std::ostringstream &macro_lines, const Location& loc,
                       const std::string& name, std::vector<std::vector<std::string>> &args_list,
                       bool is_start = false);
    void expand(std::ostringstream &macro_lines, std::vector<std::vector<std::string>> &args_list);
public:
    Macro() : level(0), ancestor(nullptr) {
        endm_pat = std::regex(R"(^\s*ENDM\s*([;].*)?$)", std::regex::icase);
        mac = std::regex(R"(^\s*([_a-zA-Z][_a-zA-Z0-9]*)\s+[mM][aA][cC][rR][oO]\s*([;].*)?$)");
        expand_pat = std::regex(R"(^\s([_a-zA-Z][_a-zA-Z0-9]*\s+)?[.]([_a-zA-Z][_a-zA-Z0-9]*))");
        arg_pat = std::regex(R"(^\s*(`[^`]*`(\s*[,]\s*`[^`]`)*)\s*([;].*)?$)");

    };
    Macro(Macro *parent, int lev) : level(lev), ancestor(parent) {};
    void addMacro(SourceItem &source, const std::string& root_name);
    std::string expandMacro(Location *&loc, const std::string& name, const std::vector<std::string>& args);
};


#endif //CASM_MACRO_H

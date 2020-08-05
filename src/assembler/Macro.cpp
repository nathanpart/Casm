//
// Created by nathan on 8/4/20.
//

#include <sstream>
#include "Macro.h"
#include "Assembler.h"

using namespace std;

void Macro::addNested(SourceItem &source, const std::string& root_name) {
    if (nestedMacros.count(root_name) > 0) {
        throw CasmErrorException("Macro name already in use.", source.getLocation());
    }
    Macro root = Macro(this, level + 1);
    root.addMacro(source, root_name);
    nestedMacros[root_name] = root;
}

void Macro::addMacro(SourceItem &source, const string& root_name) {
    if (ancestor == nullptr) {
        addNested(source, root_name);
        return;
    }

    location = source.getLocation();
    macro_name = root_name;
    string line;
    smatch results;

    while (true) {
        line = source.getLine();
        if (source.atEof()) {
            throw CasmErrorException("File ended before closing macro definition.", source.getLocation());
        }
        if (regex_search(line, results, endm_pat)) {
            return;
        }
        if (regex_search(line, results, mac)) {
            addNested(source, results.str(1));
        }
        else {
            macroLines.push_back(line);
        }
    }
}

std::string Macro::expandMacro(Location *&loc, const std::string& name, const std::vector<std::string>& args) {
    expansion_count = 0;
    ostringstream macro_lines;
    vector<vector<string>> arg_list;
    arg_list.push_back(args);
    doExpandMacro(macro_lines, *loc, name, arg_list);
    loc = &startLoc;
    return macro_lines.str();
}

void Macro::doExpandMacro(ostringstream &macro_lines, const Location& loc,
                          const std::string& name, vector<vector<string>> &args_list, bool is_start) {
    if (nestedMacros.count(name) == 0) {
        if (ancestor == nullptr) {
            throw CasmErrorException("Macro not found.", loc);
        }
        ancestor->doExpandMacro(macro_lines, loc, name, args_list);
        return;
    }
    if (is_start) {
        startLoc = nestedMacros[name].location;
    }
    nestedMacros[name].expand(macro_lines, args_list);
}

void Macro::expand(ostringstream &macro_lines, vector<std::vector<std::string>> &args_list) {
    int brace_count;
    Location loc = location;

    for (auto &str: macroLines) {
        loc.lineNumber++;
        string match_str = str;
        string out_str;
        brace_count = 0;
        for (auto& args: args_list) {
            brace_count++;
            regex pattern = regex("^[{]{" + to_string(brace_count) + "}\\s*([1-9][0-9]*)\\s*[}]{" +
                    to_string(brace_count) + "}");
            smatch results;

            auto brace_pos = match_str.find('{');
            while (!match_str.empty()) {
                out_str = match_str.substr(0, brace_pos);
                match_str = match_str.substr(brace_pos);
                if (not regex_search(match_str, results, pattern))
                    break;
                int item_idx = stoi(results.str(1)) - 1;
                if (item_idx < args.size()) {
                    auto rep = args[item_idx];
                    out_str += rep;
                    match_str = results.suffix();
                }
                else {
                    throw CasmErrorException("Missing macro argument.", loc);
                }
            }
            if (!match_str.empty()) {
                out_str += match_str;
            }
            match_str = out_str;
        }
        smatch results;
        // Output the line and continue with the next line if not invoking a macro
        if (not regex_search(match_str, results, expand_pat)) {
            macro_lines << match_str << "\n";
            continue;
        }
        // Output the label on its own line if it exists.  The rest of the line is the macro invoking
        if (!results.str(1).empty()) {
            macro_lines << results.str(1) << "\n";
        }
        string expand_name = results.str(2);
        string suffix = results.suffix();
        if (not regex_search(suffix, results, arg_pat)) {
            throw CasmErrorException("Error in macro arguments.", location);
        }
        vector<string> exp_args;
        string args_string = results.str(1);
        while(regex_search(args_string, results, regex(R"(`[^`]*`)"))) {
            exp_args.push_back(results.str(0));
        }
        if (!exp_args.empty()) {
            args_list.push_back(exp_args);
        }
        if (expansion_count > 100) {
            throw CasmErrorException("Recursive macro expansion.", location);
        }
        expansion_count++;
        doExpandMacro(macro_lines, loc, expand_name, args_list);
        expansion_count--;
        if (!exp_args.empty()) {
            args_list.pop_back();
        }
    }
}

//
// Created by nathan on 10/24/20.
//


#include <iostream>
#include "assembler/SourceItem.h"
#include "assembler/Error.h"
#include "Parser/token.h"

int main(int argc, char **args) {

    if (argc < 2) {
        std::cout << "Missing filename. \n\n Usage:  dump_nodes filename\n\n";
    }

    try {
        auto src_item = SourceItem(args[1], SourceType::MAIN_FILE);
        try {
            std::shared_ptr <node> tree;
            std::string line_text;

            do {
                tie(tree, line_text) = src_item.getParsedLine();
                std::cout << "\n" << line_text << "\n";
                tree->listTree();
            } while (tree->child.back().type != ENDMARKER);
        }
        catch (CasmErrorException &ce) {
            printErrorMsg(ce.what(), ce.getLocation(), ce.getLine());
        }

    }
    catch (CasmErrorException &ce) {
        printErrorMsg(ce.what(), ce.getLocation(), ce.getLine());
        return -1;
    }

    return 0;
}


//
// Created by nathan on 7/22/20.
//

#ifndef CASM_LABEL_H
#define CASM_LABEL_H

#include <vector>
#include <string>

struct Label;

class Labels {
    std::vector<Label> labels;

    std::pair<std::vector<Label>::iterator, std::vector<Label>::iterator> findLocalRange(int refAdr);
public:
    bool addLabel(const std::string& name, bool local, int location, bool is_absolute);
    bool findLocal(const std::string& name, int refAddress, int &address, bool &is_absolute);
    bool findLabel(const std::string& name, int &address, bool &is_absolute);
    bool hasLocal(const std::string& name, int refAddress);
    bool hasLabel(const std::string& name);
};

struct Label {
    std::string name;
    bool isLocal;
    int address;
    bool isAbsolute;
};

#endif //CASM_LABEL_H

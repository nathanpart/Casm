//
// Created by nathan on 7/22/20.
//

#include <tuple>
#include "Label.h"

using namespace std;

bool Labels::addLabel(const string& name, bool local, int location, bool is_absolute) {
    if (local ? hasLocal(name, location) : hasLabel(name))
        return false;
    Label label;
    label.address = location;
    label.isLocal = local;
    label.name = name;
    label.isAbsolute = is_absolute;
    labels.push_back(label);
    return true;
}

bool Labels::findLocal(const string& name, int refAddress, int &address, bool &is_absolute) {
    vector<Label>::iterator l_begin;
    vector<Label>::iterator l_end;

    tie(l_begin, l_end) = findLocalRange(refAddress);
    for (auto it = l_begin; it != l_end; it++) {
        if (it->isLocal && it->name == name) {
            address = it->address;
            is_absolute = it->isAbsolute;
            return true;
        }
    }
    return false;
}

bool Labels::findLabel(const string& name, int &address, bool &is_absolute) {
    for (const auto& label: labels) {
        if ((!label.isLocal) && label.name == name) {
            address = label.address;
            is_absolute = label.isAbsolute;
            return true;
        }
    }
    return false;
}

bool Labels::hasLocal(const string& name, int refAddress) {
    int x = 0;
    bool f;
    return findLocal(name, refAddress, x, f);
}

bool Labels::hasLabel(const string& name) {
    int x = 0;
    bool b;
    return findLabel(name, x, b);
}

pair<vector<Label>::iterator, vector<Label>::iterator>
Labels::findLocalRange(int refAdr) {
    auto low_location = labels.begin();
    int global_low = low_location->address;

    for (auto it = labels.begin(); it != labels.end(); it++) {
        if ((!it->isLocal) && it->address > global_low) {
            low_location = it;
            global_low = it->address;
        }
        if ((!it->isLocal) && it->address >= refAdr) {
            it++;
            return pair<vector<Label>::iterator, vector<Label>::iterator>(low_location, it);
        }
    }
    return pair<vector<Label>::iterator, vector<Label>::iterator>(low_location, labels.end());
}

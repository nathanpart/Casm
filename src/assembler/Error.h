//
// Created by nathan on 8/6/20.
//

#ifndef CASM_ERROR_H
#define CASM_ERROR_H

#include <string>
#include <exception>

#include "Location.h"

class CasmErrorException : public std::exception {
    const std::string error_msg;
    const std::string line;
    Location location;

public:
    CasmErrorException(std::string  err_mag, Location loc, std::string err_line = "")
            : error_msg(std::move(err_mag)), location(std::move(loc)), line(std::move(err_line)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return error_msg.c_str();
    }

    Location getLocation() {
        return location;
    }

    [[nodiscard]] const char * getLine() const noexcept {
        return line.c_str();
    }
};

void printErrorMsg(const std::string& msg, const Location& location, const std::string& lineText);
void printWarnMsg(const std::string& msg, const Location& location, const std::string& lineText);

#endif //CASM_ERROR_H

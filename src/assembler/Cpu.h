//
// Created by nathan on 8/13/20.
//

#ifndef CASM_CPU_H
#define CASM_CPU_H

#include <string>
#include <set>
#include <map>
#include <tuple>
#include <regex>

#include "AddressMode.h"
enum class CpuMnemonics {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL,
    BRA, BRK, BRL, BVC, BVS, CLC, CLD, CLI, CLV, CMP,
    COP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY,
    JMP, JML, JSL, JSR, LDA, LDX, LDY, LSR, MVN, MVP,
    NOP, ORA, PEA, PEI, PER, PHA, PHB, PHD, PHK, PHP,
    PHX, PHY, PLA, PLB, PLD, PLP, PLX, PLY, REP, ROL,
    ROR, RTI, RTL, RTS, SBC, SEC, SED, SEI, SEP, STA,
    STP, STX, STY, STZ, TAX, TAY, TCD, TCS, TDC, TRB,
    TSB, TSC, TSX, TXA, TXS, TXY, TYA, TYX, WAI, WDM,
    XBA, XCE,
    BBR0, BBR1, BBR2, BBR3, BBR4, BBR5, BBR6, BBR7,
    BBS0, BBS1, BBS2, BBS3, BBS4, BBS5, BBS6, BBS7,
    RMB0, RMB1, RMB2, RMB3, RMB4, RMB5, RMB6, RMB7,
    SMB0, SMB1, SMB2, SMB3, SMB4, SMB5, SMB6, SMB7
};

enum class CpuType {CPU_6502, CPU_65C02, CPU_65C816, CPU_R65C02};

using CpuOpcode = std::tuple<uint8_t, std::set<CpuType>>;
using CpuOpcodes = std::map<AddressModes, CpuOpcode>;
using CpuInstrItem = std::tuple<CpuOpcodes, std::set<CpuType>, std::regex, char>;
using CpuInstrMap = std::map<CpuMnemonics, CpuInstrItem>;

extern CpuInstrMap cpuInstrMap;

void cpuInstrMapInit();


#endif //CASM_CPU_H

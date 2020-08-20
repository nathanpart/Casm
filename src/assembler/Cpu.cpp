//
// Created by nathan on 8/13/20.
//

#include "Cpu.h"

using namespace std;

CpuInstrMap cpuInstrMap;

static bool init_flag = false;

void cpuInstrMapInit() {
    if (init_flag) return;
    init_flag = true;

    set<CpuType> cpu_all = {CpuType::CPU_6502, CpuType::CPU_65C02, CpuType::CPU_R65C02, CpuType::CPU_65C816};
    set<CpuType> cpu_big = {CpuType::CPU_65C02, CpuType::CPU_R65C02, CpuType::CPU_65C816};
    set<CpuType> cpu_R65C02 = {CpuType::CPU_R65C02};
    set<CpuType> cpu_65C816 = {CpuType::CPU_65C816};

    CpuInstrItem cpu_instr = {{
                                      {AddressModes::imm, {0x69u, cpu_all}},
                                      {AddressModes::abs, {0x6Du, cpu_all}},
                                      {AddressModes::abs_long, {0x6Fu, cpu_65C816}},
                                      {AddressModes::dp, {0x65u, cpu_all}},
                                      {AddressModes::dp_ind, {0x72u, cpu_big}},
                                      {AddressModes::dp_ind_long, {0x67u, cpu_65C816}},
                                      {AddressModes::abs_x, {0x7Du, cpu_all}},
                                      {AddressModes::abs_long_x, {0x7Fu, cpu_65C816}},
                                      {AddressModes::abs_y, {0x79u, cpu_all}},
                                      {AddressModes::dp_x, {0x75u, cpu_all}},
                                      {AddressModes::dp_ind_x, {0x61u, cpu_all}},
                                      {AddressModes::dp_ind_y, {0x71u, cpu_all}},
                                      {AddressModes::dp_ind_long_y, {0x77u, cpu_65C816}},
                                      {AddressModes::sr, {0x63u, cpu_65C816}},
                                      {AddressModes::sr_ind_y, {0x73u, cpu_65C816}}
                              }, cpu_all, regex("adc", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::ADC] = cpu_instr;

    cpu_instr = {{
                                    {AddressModes::imm, {0x29u, cpu_all}},
                                    {AddressModes::abs, {0x2Du, cpu_all}},
                                    {AddressModes::abs_long, {0x2Fu, cpu_65C816}},
                                    {AddressModes::dp, {0x25u, cpu_all}},
                                    {AddressModes::dp_ind, {0x32u, cpu_big}},
                                    {AddressModes::dp_ind_long, {0x27u, cpu_65C816}},
                                    {AddressModes::abs_x, {0x3Du, cpu_all}},
                                    {AddressModes::abs_long_x, {0x3Fu, cpu_65C816}},
                                    {AddressModes::abs_y, {0x39u, cpu_all}},
                                    {AddressModes::dp_x, {0x35u, cpu_all}},
                                    {AddressModes::dp_ind_x, {0x21u, cpu_all}},
                                    {AddressModes::dp_ind_y, {0x31u, cpu_all}},
                                    {AddressModes::dp_ind_long_y, {0x37u, cpu_65C816}},
                                    {AddressModes::sr, {0x23u, cpu_65C816}},
                                    {AddressModes::sr_ind_y, {0x33u, cpu_65C816}}
                            }, cpu_all, regex("and", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::AND] = cpu_instr;

    cpu_instr = {{
                                    {AddressModes::acc, {0x0Au, cpu_all}},
                                    {AddressModes::abs, {0x0Eu, cpu_all}},
                                    {AddressModes::dp, {0x06u, cpu_all}},
                                    {AddressModes::abs_x, {0x1E, cpu_all}},
                                    {AddressModes::dp_x, {0x16u, cpu_all}}
                            }, cpu_all, regex("asl", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::ASL] = cpu_instr;

    cpu_instr = {{
                                    {AddressModes::rel, {0x90u, cpu_all}}
                            }, cpu_all, regex("(bcc)|(blt)", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BCC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0xB0u, cpu_all}}
                 }, cpu_all, regex("(bcs)|(bge)", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BCS] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0xF0u, cpu_all}}
                 }, cpu_all, regex("beq", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BEQ] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0x89u, cpu_big}},
                         {AddressModes::abs, {0x2Cu, cpu_all}},
                         {AddressModes::dp, {0x24u, cpu_all}},
                         {AddressModes::abs_x, {0x3Cu, cpu_big}},
                         {AddressModes::dp_x, {0x34u, cpu_big}}
                 }, cpu_all, regex("bit", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::BIT] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0x30u, cpu_all}}
                 }, cpu_all, regex("bmi", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BMI] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0xD0u, cpu_all}}
                 }, cpu_all, regex("bne", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BNE] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0x10u, cpu_all}}
                 }, cpu_all, regex("bpl", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BPL] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0x80u, cpu_big}}
                 }, cpu_big, regex("bra", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BRA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x00u, cpu_all}},
                         {AddressModes::imm, {0x00u, cpu_all}}
                 }, cpu_all, regex("brk", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BRK] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel_long, {0x82u, cpu_65C816}}
                 }, cpu_65C816, regex("brl", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BRL] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0x50u, cpu_all}}
                 }, cpu_all, regex("bvc", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BVC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::rel, {0x70u, cpu_all}}
                 }, cpu_all, regex("bvs", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BVS] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x18u, cpu_all}}
                 }, cpu_all, regex("clc", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::CLC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xD8u, cpu_all}}
                 }, cpu_all, regex("cld", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::CLD] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x58u, cpu_all}}
                 }, cpu_all, regex("cli", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::CLI] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xB8u, cpu_all}}
                 }, cpu_all, regex("clv", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::CLV] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xC9u, cpu_all}},
                         {AddressModes::abs, {0xCDu, cpu_all}},
                         {AddressModes::abs_long, {0xCFu, cpu_65C816}},
                         {AddressModes::dp, {0xC5u, cpu_all}},
                         {AddressModes::dp_ind, {0xD2u, cpu_big}},
                         {AddressModes::dp_ind_long, {0xC7u, cpu_65C816}},
                         {AddressModes::abs_x, {0xDDu, cpu_all}},
                         {AddressModes::abs_long_x, {0xDFu, cpu_65C816}},
                         {AddressModes::abs_y, {0xD9u, cpu_all}},
                         {AddressModes::dp_x, {0xD5u, cpu_all}},
                         {AddressModes::dp_ind_x, {0xC1u, cpu_all}},
                         {AddressModes::dp_ind_y, {0xD1u, cpu_all}},
                         {AddressModes::dp_ind_long_y, {0xD7u, cpu_65C816}},
                         {AddressModes::sr, {0xC3u, cpu_65C816}},
                         {AddressModes::sr_ind_y, {0xD3u, cpu_65C816}}
                 }, cpu_all, regex("cmp", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::CMP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x02u, cpu_65C816}},
                         {AddressModes::imm, {0x02u, cpu_65C816}}
                 }, cpu_65C816, regex("cop", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::COP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xE0u, cpu_all}},
                         {AddressModes::abs, {0xECu, cpu_all}},
                         {AddressModes::dp, {0xE4u, cpu_all}},
                 }, cpu_all, regex("cpx", regex::icase), 'X'};
    cpuInstrMap[CpuMnemonics::CPX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xC0u, cpu_all}},
                         {AddressModes::abs, {0xCCu, cpu_all}},
                         {AddressModes::dp, {0xC4u, cpu_all}},
                 }, cpu_all, regex("cpy", regex::icase), 'X'};
    cpuInstrMap[CpuMnemonics::CPY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::acc, {0x3Au, cpu_big}},
                         {AddressModes::abs, {0xCEu, cpu_all}},
                         {AddressModes::dp, {0xC6u, cpu_all}},
                         {AddressModes::abs_x, {0xDE, cpu_all}},
                         {AddressModes::dp_x, {0xD6u, cpu_all}}
                 }, cpu_all, regex("dec", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::DEC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xCAu, cpu_all}}
                 }, cpu_all, regex("dex", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::DEX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x88u, cpu_all}}
                 }, cpu_all, regex("dey", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::DEY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0x49u, cpu_all}},
                         {AddressModes::abs, {0x4Du, cpu_all}},
                         {AddressModes::abs_long, {0x4Fu, cpu_65C816}},
                         {AddressModes::dp, {0x45u, cpu_all}},
                         {AddressModes::dp_ind, {0x52u, cpu_big}},
                         {AddressModes::dp_ind_long, {0x47u, cpu_65C816}},
                         {AddressModes::abs_x, {0x5Du, cpu_all}},
                         {AddressModes::abs_long_x, {0x5Fu, cpu_65C816}},
                         {AddressModes::abs_y, {0x59u, cpu_all}},
                         {AddressModes::dp_x, {0x55u, cpu_all}},
                         {AddressModes::dp_ind_x, {0x41u, cpu_all}},
                         {AddressModes::dp_ind_y, {0x51u, cpu_all}},
                         {AddressModes::dp_ind_long_y, {0x57u, cpu_65C816}},
                         {AddressModes::sr, {0x43u, cpu_65C816}},
                         {AddressModes::sr_ind_y, {0x53u, cpu_65C816}}
                 }, cpu_all, regex("eor", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::EOR] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::acc, {0x1Au, cpu_big}},
                         {AddressModes::abs, {0xEEu, cpu_all}},
                         {AddressModes::dp, {0xE6u, cpu_all}},
                         {AddressModes::abs_x, {0xFE, cpu_all}},
                         {AddressModes::dp_x, {0xF6u, cpu_all}}
                 }, cpu_all, regex("inc", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::INC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xEAu, cpu_all}}
                 }, cpu_all, regex("inx", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::INX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xC8u, cpu_all}}
                 }, cpu_all, regex("iny", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::INY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x4Cu, cpu_big}},
                         {AddressModes::abs_ind, {0x6Cu, cpu_all}},
                         {AddressModes::abs_ind_x, {0x7Cu, cpu_big}},
                         {AddressModes::abs_long, {0x5C, cpu_65C816}},
                         {AddressModes::dp_ind_long, {0xDCu, cpu_65C816}}
                 }, cpu_all, regex("jmp", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::JMP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs_long, {0x5C, cpu_65C816}},
                         {AddressModes::dp_ind_long, {0xDCu, cpu_65C816}}
                 }, cpu_65C816, regex("jml", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::JML] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs_long, {0x22, cpu_65C816}},
                 }, cpu_65C816, regex("jsl", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::JSL] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x20u, cpu_all}},
                         {AddressModes::abs_ind_x, {0xFCu, cpu_big}},
                         {AddressModes::abs_long, {0x22, cpu_65C816}},
                 }, cpu_all, regex("jsr", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::JSR] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xA9u, cpu_all}},
                         {AddressModes::abs, {0xADu, cpu_all}},
                         {AddressModes::abs_long, {0xAFu, cpu_65C816}},
                         {AddressModes::dp, {0xA5u, cpu_all}},
                         {AddressModes::dp_ind, {0xB2u, cpu_big}},
                         {AddressModes::dp_ind_long, {0xA7u, cpu_65C816}},
                         {AddressModes::abs_x, {0xBDu, cpu_all}},
                         {AddressModes::abs_long_x, {0xBFu, cpu_65C816}},
                         {AddressModes::abs_y, {0xB9u, cpu_all}},
                         {AddressModes::dp_x, {0xB5u, cpu_all}},
                         {AddressModes::dp_ind_x, {0xA1u, cpu_all}},
                         {AddressModes::dp_ind_y, {0xB1u, cpu_all}},
                         {AddressModes::dp_ind_long_y, {0xB7u, cpu_65C816}},
                         {AddressModes::sr, {0xA3u, cpu_65C816}},
                         {AddressModes::sr_ind_y, {0xB3u, cpu_65C816}}
                 }, cpu_all, regex("lda", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::LDA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xA2u, cpu_big}},
                         {AddressModes::abs, {0xAEu, cpu_all}},
                         {AddressModes::dp, {0xA6u, cpu_all}},
                         {AddressModes::abs_x, {0xBE, cpu_all}},
                         {AddressModes::dp_x, {0xB6u, cpu_all}}
                 }, cpu_all, regex("ldx", regex::icase), 'X'};
    cpuInstrMap[CpuMnemonics::LDX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xA0u, cpu_big}},
                         {AddressModes::abs, {0xACu, cpu_all}},
                         {AddressModes::dp, {0xA4u, cpu_all}},
                         {AddressModes::abs_x, {0xBC, cpu_all}},
                         {AddressModes::dp_x, {0xB4u, cpu_all}}
                 }, cpu_all, regex("ldy", regex::icase), 'X'};
    cpuInstrMap[CpuMnemonics::LDY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::acc, {0x4Au, cpu_all}},
                         {AddressModes::abs, {0x4Eu, cpu_all}},
                         {AddressModes::dp, {0x46u, cpu_all}},
                         {AddressModes::abs_x, {0x5E, cpu_all}},
                         {AddressModes::dp_x, {0x56u, cpu_all}}
                 }, cpu_all, regex("lsr", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::LSR] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::block, {0x54u, cpu_65C816}}
                 }, cpu_65C816, regex("mvn", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::MVN] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::block, {0x44u, cpu_65C816}}
                 }, cpu_65C816, regex("mvp", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::MVP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xEAu, cpu_all}}
                 }, cpu_all, regex("nop", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::NOP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0x09u, cpu_all}},
                         {AddressModes::abs, {0x0Du, cpu_all}},
                         {AddressModes::abs_long, {0x0Fu, cpu_65C816}},
                         {AddressModes::dp, {0x05u, cpu_all}},
                         {AddressModes::dp_ind, {0x12u, cpu_big}},
                         {AddressModes::dp_ind_long, {0x07u, cpu_65C816}},
                         {AddressModes::abs_x, {0x1Du, cpu_all}},
                         {AddressModes::abs_long_x, {0x1Fu, cpu_65C816}},
                         {AddressModes::abs_y, {0x19u, cpu_all}},
                         {AddressModes::dp_x, {0x15u, cpu_all}},
                         {AddressModes::dp_ind_x, {0x01u, cpu_all}},
                         {AddressModes::dp_ind_y, {0x11u, cpu_all}},
                         {AddressModes::dp_ind_long_y, {0x17u, cpu_65C816}},
                         {AddressModes::sr, {0x03u, cpu_65C816}},
                         {AddressModes::sr_ind_y, {0x13u, cpu_65C816}}
                 }, cpu_all, regex("ora", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::ORA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0xF4u, cpu_65C816}}
                 }, cpu_65C816, regex("pea", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PEA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_ind, {0xD4u, cpu_65C816}}
                 }, cpu_65C816, regex("pei", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PEI] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x62u, cpu_65C816}}
                 }, cpu_65C816, regex("per", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PER] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x48u, cpu_all}}
                 }, cpu_all, regex("pha", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PHA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x8Bu, cpu_65C816}}
                 }, cpu_65C816, regex("phb", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PHB] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x0Bu, cpu_65C816}}
                 }, cpu_65C816, regex("phd", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PHD] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x4Bu, cpu_65C816}}
                 }, cpu_65C816, regex("phk", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PHK] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x08u, cpu_all}}
                 }, cpu_all, regex("php", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PHP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xDAu, cpu_big}}
                 }, cpu_big, regex("phx", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PHX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x5Au, cpu_big}}
                 }, cpu_big, regex("phy", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PHY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x68u, cpu_all}}
                 }, cpu_all, regex("pla", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PLA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xABu, cpu_65C816}}
                 }, cpu_65C816, regex("plb", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PLB] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x2Bu, cpu_65C816}}
                 }, cpu_65C816, regex("pld", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PLD] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x28u, cpu_all}}
                 }, cpu_all, regex("plp", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PLP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xFAu, cpu_big}}
                 }, cpu_big, regex("plx", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PLX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x7Au, cpu_big}}
                 }, cpu_big, regex("ply", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::PLY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xC2u, cpu_65C816}}
                 }, cpu_65C816, regex("rep", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::REP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::acc, {0x2Au, cpu_all}},
                         {AddressModes::abs, {0x2Eu, cpu_all}},
                         {AddressModes::dp, {0x26u, cpu_all}},
                         {AddressModes::abs_x, {0x3E, cpu_all}},
                         {AddressModes::dp_x, {0x36u, cpu_all}}
                 }, cpu_all, regex("rol", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::ROL] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::acc, {0x6Au, cpu_all}},
                         {AddressModes::abs, {0x6Eu, cpu_all}},
                         {AddressModes::dp, {0x66u, cpu_all}},
                         {AddressModes::abs_x, {0x7E, cpu_all}},
                         {AddressModes::dp_x, {0x76u, cpu_all}}
                 }, cpu_all, regex("ror", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::ROR] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x40u, cpu_all}}
                 }, cpu_all, regex("rti", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RTI] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x6Bu, cpu_65C816}}
                 }, cpu_65C816, regex("rtl", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RTL] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x60u, cpu_all}}
                 }, cpu_all, regex("rts", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RTS] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xE9u, cpu_all}},
                         {AddressModes::abs, {0xEDu, cpu_all}},
                         {AddressModes::abs_long, {0xEFu, cpu_65C816}},
                         {AddressModes::dp, {0xE5u, cpu_all}},
                         {AddressModes::dp_ind, {0xF2u, cpu_big}},
                         {AddressModes::dp_ind_long, {0xE7u, cpu_65C816}},
                         {AddressModes::abs_x, {0xFDu, cpu_all}},
                         {AddressModes::abs_long_x, {0xFFu, cpu_65C816}},
                         {AddressModes::abs_y, {0xF9u, cpu_all}},
                         {AddressModes::dp_x, {0xF5u, cpu_all}},
                         {AddressModes::dp_ind_x, {0xE1u, cpu_all}},
                         {AddressModes::dp_ind_y, {0xF1u, cpu_all}},
                         {AddressModes::dp_ind_long_y, {0xF7u, cpu_65C816}},
                         {AddressModes::sr, {0xE3u, cpu_65C816}},
                         {AddressModes::sr_ind_y, {0xF3u, cpu_65C816}}
                 }, cpu_all, regex("sbc", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::SBC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x38u, cpu_all}}
                 }, cpu_all, regex("sec", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SEC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xF8u, cpu_all},}
                 }, cpu_all, regex("sed", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SED] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x78u, cpu_all}}
                 }, cpu_all, regex("sei", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SEI] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imm, {0xE2u, cpu_65C816}}
                 }, cpu_65C816, regex("sep", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SEP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x8Du, cpu_all}},
                         {AddressModes::abs_long, {0x8Fu, cpu_65C816}},
                         {AddressModes::dp, {0x85u, cpu_all}},
                         {AddressModes::dp_ind, {0x92u, cpu_big}},
                         {AddressModes::dp_ind_long, {0x87u, cpu_65C816}},
                         {AddressModes::abs_x, {0x9Du, cpu_all}},
                         {AddressModes::abs_long_x, {0x9Fu, cpu_65C816}},
                         {AddressModes::abs_y, {0x99u, cpu_all}},
                         {AddressModes::dp_x, {0x95u, cpu_all}},
                         {AddressModes::dp_ind_x, {0x81u, cpu_all}},
                         {AddressModes::dp_ind_y, {0x91u, cpu_all}},
                         {AddressModes::dp_ind_long_y, {0x97u, cpu_65C816}},
                         {AddressModes::sr, {0x83u, cpu_65C816}},
                         {AddressModes::sr_ind_y, {0x93u, cpu_65C816}}
                 }, cpu_all, regex("sta", regex::icase), 'A'};
    cpuInstrMap[CpuMnemonics::STA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xDBu, cpu_65C816}}
                 }, cpu_65C816, regex("stp", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::STP] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x8Eu, cpu_all}},
                         {AddressModes::dp, {0x86u, cpu_all}},
                         {AddressModes::dp_y, {0x96u, cpu_all}},
                 }, cpu_all, regex("stx", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::STX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x8Cu, cpu_all}},
                         {AddressModes::dp, {0x84u, cpu_all}},
                         {AddressModes::dp_x, {0x94u, cpu_all}},
                 }, cpu_all, regex("sty", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::STY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x9Cu, cpu_big}},
                         {AddressModes::dp, {0x64u, cpu_big}},
                         {AddressModes::abs_x, {0x9Eu, cpu_big}},
                         {AddressModes::dp_x, {0x74u, cpu_big}},
                 }, cpu_big, regex("stz", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::STZ] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xAAu, cpu_all}}
                 }, cpu_all, regex("tax", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TAX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xA8u, cpu_all}}
                 }, cpu_all, regex("tay", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TAY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x5Bu, cpu_65C816}}
                 }, cpu_65C816, regex("(tcd)|(tad)", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TCD] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x1Bu, cpu_all}}
                 }, cpu_all, regex("(tcs)|(tas)", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TCS] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x7Bu, cpu_65C816}}
                 }, cpu_65C816, regex("(tdc)|(tda)", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TDC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x1Cu, cpu_big}},
                         {AddressModes::dp, {0x14, cpu_big}},
                 }, cpu_big, regex("trb", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TRB] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::abs, {0x0Cu, cpu_big}},
                         {AddressModes::dp, {0x04, cpu_big}},
                 }, cpu_big, regex("tsb", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TSB] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x3Bu, cpu_65C816}}
                 }, cpu_65C816, regex("(tsc)|(tsa)", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TSC] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xBAu, cpu_all}}
                 }, cpu_all, regex("tsx", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TSX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x8Au, cpu_all}}
                 }, cpu_all, regex("txa", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TXA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x9Au, cpu_all}}
                 }, cpu_all, regex("txs", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TXS] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x9Bu, cpu_all}}
                 }, cpu_all, regex("txy", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TXY] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x98u, cpu_all}}
                 }, cpu_all, regex("tya", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TYA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xBBu, cpu_65C816}}
                 }, cpu_65C816, regex("tyx", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::TYX] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xCBu, cpu_65C816}}
                 }, cpu_65C816, regex("wai", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::WAI] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0x42u, cpu_65C816}}
                 }, cpu_65C816, regex("wdm", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::WDM] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xEBu, cpu_65C816}}
                 }, cpu_65C816, regex("xba", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::XBA] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::imp, {0xFBu, cpu_65C816}}
                 }, cpu_65C816, regex("xce", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::XCE] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x0Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr0", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR0] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x1Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr1", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR1] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x2Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr2", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR2] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x3Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr3", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR3] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x4Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr4", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR4] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x5Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr5", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR5] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x6Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr6", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR6] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x7Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbr7", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBR7] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x8Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs0", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS0] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0x9Fu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs1", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS1] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0xAFu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs2", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS2] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0xBFu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs3", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS3] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0xCFu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs4", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS4] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0xDFu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs5", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS5] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0xEFu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs6", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS6] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp_rel, {0xFFu, cpu_R65C02}}
                 }, cpu_R65C02, regex("bbs7", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::BBS7] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x07u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb0", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB0] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x17u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb1", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB1] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x27u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb2", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB2] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x37u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb3", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB3] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x47u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb4", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB4] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x57u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb5", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB5] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x67u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb6", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB6] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x77u, cpu_R65C02}}
                 }, cpu_R65C02, regex("rmb7", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::RMB7] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x87u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb0", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB0] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0x97u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb1", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB1] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0xA7u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb2", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB2] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0xB7u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb3", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB3] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0xC7u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb4", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB4] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0xD7u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb5", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB5] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0xE7u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb6", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB6] = cpu_instr;

    cpu_instr = {{
                         {AddressModes::dp, {0xF7u, cpu_R65C02}}
                 }, cpu_R65C02, regex("smb7", regex::icase), 0};
    cpuInstrMap[CpuMnemonics::SMB7] = cpu_instr;

}
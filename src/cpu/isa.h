#pragma once

enum Opcode : u8 {
    OP_NOP = 0x00,
    OP_MOV_IMM = 0x01,
    OP_MOV_REG = 0x02,
    OP_ADD = 0x03,

    OP_LOAD = 0x04,
    OP_STORE = 0x05,

    OP_PUSH = 0x06,
    OP_POP = 0x07,

    OP_CALL = 0x08,
    OP_RET = 0x09,

    OP_JMP = 0x0A,

    OP_HLT = 0xFF
};

enum Register : u8 {
    REG_AX,
    REG_BX,
    REG_CX,
    REG_DX,
    REG_SP,
    REG_IP,
    REG_FLAGS,
    REG_COUNT
};

constexpr u16 FLAG_ZF = 1 << 0;
constexpr u16 FLAG_CF = 1 << 1;
constexpr u16 FLAG_SF = 1 << 2;
#include "cpu.h"
#include <iostream>

CPU::CPU(Memory& mem) : memory(mem) {
    reset();
}

void CPU::reset() {
    for (int i = 0; i < REG_COUNT; i++) {
        regs[i] = 0;
    }

    regs[REG_SP] = 0xFFFE;
    regs[REG_IP] = 0x1000;
    halted = false;
}

bool CPU::isHalted() const {
    return halted;
}

u16 CPU::getReg(Register r) const {
    return regs[r];
}

u8 CPU::fetch8() {
    return memory.read8(regs[REG_IP]++);
}

u16 CPU::fetch16() {
    u8 lo = fetch8();
    u8 hi = fetch8();

    return lo | (hi << 8);
}

void CPU::setIP(u16 v) {
    regs[REG_IP] = v;
}

void CPU::setSP(u16 v) {
    regs[REG_SP] = v;
}

void CPU::clearRegs() {
    regs[REG_AX] = 0;
    regs[REG_BX] = 0;
    regs[REG_CX] = 0;
    regs[REG_DX] = 0;
    regs[REG_FLAGS] = 0;
}

void CPU::step() {
    if (halted) return;

    u8 opcode = fetch8();
    u8 b = fetch8();
    u8 c = fetch8();
    u8 d = fetch8();

    switch (opcode) {
    case OP_NOP:
        break;
    case OP_MOV_IMM:
        regs[b] = d;
        break;
    case OP_MOV_REG:
        regs[b] = regs[c];
        break;
    case OP_ADD: {
        u32 res = regs[b] + regs[c];
        regs[b] = res & 0xFFFF;

        regs[REG_FLAGS] = 0;
        if (regs[b] == 0) regs[REG_FLAGS] |= FLAG_ZF;
        if (res > 0xFFFF) regs[REG_FLAGS] |= FLAG_CF;
        break;
    }
    case OP_LOAD: {
        u16 addr = c | (d << 8);
        regs[b] = memory.read16(addr);
        break;
    }
    case OP_STORE: {
        u16 addr = c | (d << 8);
        u16 value = regs[b];

        if (addr == 0xFF00) {
            std::cout << (char)(value & 0xFF) << std::flush;
        }
        else if (addr == 0xFF02) {
            std::cout << std::dec << value << std::flush;
        }
        else {
            memory.write16(addr, value);
        }
        break;
    }
    case OP_PUSH:
        regs[REG_SP] -= 2;
        memory.write16(regs[REG_SP], regs[b]);
        break;
    case OP_POP:
        regs[b] = memory.read16(regs[REG_SP]);
        regs[REG_SP] += 2;
        break;
    case OP_CALL: {
        u16 addr = b | (c << 8);

        regs[REG_SP] -= 2;
        memory.write16(regs[REG_SP], regs[REG_IP]);

        regs[REG_IP] = addr;
        break;
    }
    case OP_RET:
        regs[REG_IP] = memory.read16(regs[REG_SP]);
        regs[REG_SP] += 2;
        break;
    case OP_JMP: {
        u16 addr = b | (c << 8);
        regs[REG_IP] = addr;
        break;
    }
    case OP_HLT:
        halted = true;
        break;
    default:
        std::cerr << "Unknown opcode: " << int(opcode) << "\n";
        halted = true;
        break;
    }
}
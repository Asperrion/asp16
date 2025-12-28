#pragma once
#include "../util/types.h"
#include "isa.h"
#include "../memory/memory.h"

class CPU {
public:
    CPU(Memory& mem);

    void reset();
    void step();
    bool isHalted() const;

    void setIP(u16 v);
    void setSP(u16 v);
    void clearRegs();

    u16 getReg(Register r) const;

private:
    u16 regs[REG_COUNT];
    bool halted = false;
    Memory& memory;

    u8 fetch8();
    u16 fetch16();
};
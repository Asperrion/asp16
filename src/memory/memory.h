#pragma once
#include "../util/types.h"

constexpr u32 MEM_SIZE = 65536;

class Memory {
public:
    Memory();

    u8 read8(u16 addr) const;
    void write8(u16 addr, u8 value);

    u16 read16(u16 addr) const;
    void write16(u16 addr, u16 value);

private:
    u8 data[MEM_SIZE];
};
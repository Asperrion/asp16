#include "memory.h"
#include <cstring>

Memory::Memory() {
    memset(data, 0, MEM_SIZE);
}

u8 Memory::read8(u16 addr) const {
    return data[addr];
}

void Memory::write8(u16 addr, u8 value) {
    data[addr] = value;
}

u16 Memory::read16(u16 addr) const {
    return data[addr] | (data[addr + 1] << 8);
}

void Memory::write16(u16 addr, u16 value) {
    data[addr] = value & 0xFF;
    data[addr + 1] = value >> 8;
}
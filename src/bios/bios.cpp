#include "bios.h"
#include <fstream>
#include <iostream>
#include <cstring>

#pragma pack(push, 1)
struct AspHeader {
    char magic[5];
    u16 entry;
    u16 size;
};
#pragma pack(pop)

bool loadASP(const std::string& path, Memory& mem, CPU& cpu) {
    std::ifstream file(path, std::ios::binary);

    if(!file) {
        std::cerr << "BIOS: cannot open " << path << "\n";
        return false;
    }

    AspHeader hdr{};
    file.read((char*)&hdr, sizeof(hdr));

    if(std::strncmp(hdr.magic, "ASP16", 5) != 0) {
        std::cerr << "BIOS: invalid ASP file \n";
        return false;
    }

    // загрузка программы
    for (u16 i = 0; i < hdr.size; ++i) {
        char byte;
        file.read(&byte, 1);
        mem.write8(0x1000 + i, (u8)byte);
    }

    cpu.setSP(0xFFFE);
    cpu.clearRegs();

    cpu.setIP(0x1000 + hdr.entry);

    std::cout << "BIOS: loaded " << hdr.size << " bytes, entry=0x" << std::hex << (0x1000 + hdr.entry) << "\n";
    return true;
}
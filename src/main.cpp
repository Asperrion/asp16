#include "cpu/cpu.h"
#include "memory/memory.h"
#include "bios/bios.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: asp16 <program.asp>\n";
        return 1;
    }

    const char* programPath = argv[1];

    Memory memory;
    CPU cpu(memory);

    cpu.setIP(0x0000);

    // загрузка программы
    if (!loadASP(programPath, memory, cpu)) {
        return 1;
    }

    // выполнение
    while (!cpu.isHalted()) {
        cpu.step();
    }

    return 0;
}
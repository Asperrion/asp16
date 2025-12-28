#pragma once
#include "memory/memory.h"
#include "cpu/cpu.h"
#include <string>

bool loadASP(const std::string& path, Memory& mem, CPU& cpu);
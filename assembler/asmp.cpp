#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstring>

#include "../src/cpu/isa.h"
#include "../src/util/types.h"

static std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \t");
    size_t e = s.find_last_not_of(" \t");
    if (b == std::string::npos) return "";
    return s.substr(b, e - b + 1);
}

static std::vector<std::string> split(const std::string& s, char c) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, c)) {
        out.push_back(trim(item));
    }
    return out;
}

struct Instruction {
    std::string label;
    std::string op;
    std::vector<std::string> args;
};

std::unordered_map<std::string, u8> regMap = {
    {"ax", REG_AX},
    {"bx", REG_BX},
    {"cx", REG_CX},
    {"dx", REG_DX},
    {"sp", REG_SP},
    {"ip", REG_IP}
};

static bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    char* end = nullptr;
    std::strtol(s.c_str(), &end, 0);
    return *end == 0;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: asmp input.asmp output.asp\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open " << argv[1] << "\n";
        return 1;
    }

    std::vector<Instruction> program;
    std::string line;

    // парсинг для лейблов и комментариев
    while (std::getline(in, line)) {

        size_t sc = line.find(';');
        if (sc != std::string::npos)
            line = line.substr(0, sc);

        line = trim(line);

        if (line.empty())
            continue;

        Instruction ins;

        if (line.back() == ':') {
            ins.label = line.substr(0, line.size() - 1);
            program.push_back(ins);
            continue;
        }

        auto parts = split(line, ' ');
        ins.op = parts[0];

        if (parts.size() > 1) {
            std::string rest = line.substr(ins.op.size());
            rest = trim(rest);
            ins.args = split(rest, ',');
        }

        program.push_back(ins);
    }

    // PASS 1 — собираем метки
    std::unordered_map<std::string, u16> labels;
    u16 pc = 0;

    for (auto& ins : program) {

        if (!ins.label.empty()) {
            labels[ins.label] = pc;
            continue;
        }

        pc += 4;
    }

    // PASS 2 — генерация кода
    std::vector<u8> code;

    auto emit = [&](u8 a, u8 b, u8 c, u8 d) {
        code.push_back(a);
        code.push_back(b);
        code.push_back(c);
        code.push_back(d);
    };

    for (auto& ins : program) {

        // строки с метками пропускаем
        if (ins.op.empty())
            continue;

        // mov reg, imm | reg
        if (ins.op == "mov") {
            if (ins.args.size() != 2) {
                std::cerr << "mov expects 2 operands\n";
                return 1;
            }

            if (!regMap.count(ins.args[0])) {
                std::cerr << "unknown register: " << ins.args[0] << "\n";
                return 1;
            }

            u8 r1 = regMap[ins.args[0]];

            // mov reg, reg
            if (regMap.count(ins.args[1])) {
                u8 r2 = regMap[ins.args[1]];
                emit(OP_MOV_REG, r1, r2, 0);
            }
            // mov reg, imm
            else if (isNumber(ins.args[1])) {
                u8 imm = std::stoi(ins.args[1], nullptr, 0);
                emit(OP_MOV_IMM, r1, 0, imm);
            }
            else {
                std::cerr << "invalid mov operand: " << ins.args[1] << "\n";
                return 1;
            }
        }

        // add reg, reg
        else if (ins.op == "add") {
            if (ins.args.size() != 2) {
                std::cerr << "add expects 2 operands\n";
                return 1;
            }

            if (!regMap.count(ins.args[0]) || !regMap.count(ins.args[1])) {
                std::cerr << "add expects registers\n";
                return 1;
            }

            emit(OP_ADD,
                regMap[ins.args[0]],
                regMap[ins.args[1]],
                0);
        }

        // load reg, addr|label
        else if (ins.op == "load") {
            if (ins.args.size() != 2) {
                std::cerr << "load expects 2 operands\n";
                return 1;
            }

            if (!regMap.count(ins.args[0])) {
                std::cerr << "unknown register: " << ins.args[0] << "\n";
                return 1;
            }

            u16 addr;
            if (labels.count(ins.args[1])) {
                addr = labels[ins.args[1]];
            } else if (isNumber(ins.args[1])) {
                addr = std::stoi(ins.args[1], nullptr, 0);
            } else {
                std::cerr << "unknown label/address: " << ins.args[1] << "\n";
                return 1;
            }

            emit(OP_LOAD, regMap[ins.args[0]], addr & 0xFF, addr >> 8);
        }

        // store reg, addr|label
        else if (ins.op == "store") {
            if (ins.args.size() != 2) {
                std::cerr << "store expects 2 operands\n";
                return 1;
            }

            if (!regMap.count(ins.args[0])) {
                std::cerr << "unknown register: " << ins.args[0] << "\n";
                return 1;
            }

            u16 addr;
            if (labels.count(ins.args[1])) {
                addr = labels[ins.args[1]];
            } else if (isNumber(ins.args[1])) {
                addr = std::stoi(ins.args[1], nullptr, 0);
            } else {
                std::cerr << "unknown label/address: " << ins.args[1] << "\n";
                return 1;
            }

            emit(OP_STORE, regMap[ins.args[0]], addr & 0xFF, addr >> 8);
        }

        // push / pop
        else if (ins.op == "push" || ins.op == "pop") {
            if (ins.args.size() != 1 || !regMap.count(ins.args[0])) {
                std::cerr << ins.op << " expects register\n";
                return 1;
            }

            emit(ins.op == "push" ? OP_PUSH : OP_POP,
                regMap[ins.args[0]], 0, 0);
        }

        // call / jmp addr|label
        else if (ins.op == "call" || ins.op == "jmp") {
            if (ins.args.size() != 1) {
                std::cerr << ins.op << " expects 1 operand\n";
                return 1;
            }

            u16 addr;
            if (labels.count(ins.args[0])) {
                addr = labels[ins.args[0]];
            } else if (isNumber(ins.args[0])) {
                addr = std::stoi(ins.args[0], nullptr, 0);
            } else {
                std::cerr << "unknown label/address: " << ins.args[0] << "\n";
                return 1;
            }

            emit(ins.op == "call" ? OP_CALL : OP_JMP, addr & 0xFF, addr >> 8, 0);
        }

        // ret / nop / hlt
        else if (ins.op == "ret") emit(OP_RET, 0, 0, 0);
        else if (ins.op == "nop") emit(OP_NOP, 0, 0, 0);
        else if (ins.op == "hlt") emit(OP_HLT, 0, 0, 0);

        else {
            std::cerr << "unknown instruction: " << ins.op << "\n";
            return 1;
        }
    }


    // вывод служебной информации
    std::ofstream out(argv[2], std::ios::binary);

    out.write("ASP16", 5); // добавление первых 5 байт - подпись бинарника

    u16 entry = 0;
    u16 size  = code.size();

    out.write((char*)&entry, 2);
    out.write((char*)&size, 2);
    out.write((char*)code.data(), size);

    std::cout << "assembled " << size << " bytes\n";
    return 0;
}

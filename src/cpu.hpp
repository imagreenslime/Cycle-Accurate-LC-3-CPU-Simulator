#pragma once
#include <vector>
#include <cstdint>
#include "isa.hpp"   // Instruction, Opcode, fields (rd, rs1, rs2, imm), etc.
#include "cache.hpp" // Cache class


class CPU {
public:
    explicit CPU(std::vector<Instruction> program);

    void run(int max_steps = 1000000); // safety cap

    // optional helpers for tests
    int32_t reg(int i) const { return regs_[i]; }
    int32_t mem_word(uint32_t addr) const;

private:
    std::vector<Instruction> program_; // program memory
    int32_t regs_[32] = {0}; // 32 registers
    std::vector<int32_t> memory_; // word-addressed memory
    Cache cache_{memory_.data()}; // cache with backing memory
    int pc_ = 0;
    bool running_ = true;

    Instruction fetch();

    void execute(const Instruction& instr);

    void enforce_x0();
};

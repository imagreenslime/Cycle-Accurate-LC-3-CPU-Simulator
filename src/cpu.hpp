#pragma once
#include <vector>
#include <cstdint>
#include "isa.hpp"   // Instruction, Opcode, fields (rd, rs1, rs2, imm), etc.
#include "cache.hpp" // Cache class
#include "pipeline.hpp"

class CPU {
public:
    uint64_t stalls() const { return stall_count_; }
    uint64_t flushes() const { return flush_count_; }
    
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

    uint64_t cycle_;  
    uint64_t instr_count_;
    uint64_t total_cycles_;
    int pc_ = 0;
    bool running_ = true;


    Instruction fetch();

    void execute(const Instruction& instr);
    void enforce_x0();

    // pipeline stages
    void step();

    IF_ID if_id_;
    ID_EX id_ex_;
    EX_MEM ex_mem_;
    MEM_WB mem_wb_;
    // hazard predictors 
    bool writes_rd(const Instruction& instr);
    bool has_dependency(uint8_t r, const Instruction& older);
    void print_instr(const char* stage, const Instruction& instr);
    uint64_t stall_count_ = 0;
    uint64_t flush_count_ = 0;

    bool branch_taken_ = false;
    int  branch_target_ = 0;


};

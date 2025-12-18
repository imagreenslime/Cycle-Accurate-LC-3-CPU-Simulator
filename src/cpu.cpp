#include "cpu.hpp"
#include "cache.hpp"
#include <iostream>
#include <stdexcept>

CPU::CPU(std::vector<Instruction> program)
    : program_(std::move(program)),
      memory_(1 << 20, 0), // 1M words
      cache_(memory_.data())
{}

Instruction CPU::fetch() {
    // handle halt + out of bounds
    if (pc_ < 0 || pc_ >= (int)program_.size()) {
        Instruction halt{};
        halt.op = Opcode::HALT; 
        halt.valid = true;
        halt.pc = pc_;
        return halt;
    }

    Instruction instr = program_[pc_];
    instr.valid = true;
    instr.pc = pc_;
    pc_++;
    return instr;
}

void CPU::run(int max_steps) {
    printf("loading run\n");
    int steps = 0;
    running_ = true;

    while (running_) {
        if (steps++ >= max_steps) {
            throw std::runtime_error("Max steps exceeded (possible infinite loop)");
        }
        Instruction instr = fetch();
        execute(instr);
        enforce_x0();
    }

    printf("finished steps\n\n");
}

void CPU::execute(const Instruction& instr) {
    if (!instr.valid) return;

    printf("OP=%d ", (int)instr.op);
    printf("RD=%d ", instr.rd);
    printf("RS1=%d ", regs_[instr.rs1]);
    printf("RS2=%d ", regs_[instr.rs2]);
    printf("IMM=%d\n", instr.imm);


    switch (instr.op) {
        case Opcode::ADD: {
            // 
            regs_[instr.rd] = regs_[instr.rs1] + regs_[instr.rs2];
            break;
        }
        case Opcode::SUB: {
            regs_[instr.rd] = regs_[instr.rs1] - regs_[instr.rs2];
            break;
        }
        case Opcode::ADDI: { // if you have it
            regs_[instr.rd] = regs_[instr.rs1] + instr.imm;
            break;
        }
        case Opcode::LOAD: {
            // Define your addressing convention.
            // Here: effective address = regs[rs1] + imm, measured in WORD indices.
            uint32_t addr = (uint32_t)(regs_[instr.rs1] + instr.imm);
            int32_t val;
            int latency = cache_.load(addr, val);

            regs_[instr.rd] = val;
            printf("LOAD latency: %d cycles\n", latency);
            
            break;
        }
        case Opcode::STORE: {
            uint32_t addr = (uint32_t)(regs_[instr.rs1] + instr.imm);
            int32_t val = regs_[instr.rd];

            int latency = cache_.store(addr, val);
            printf("STORE latency: %d cycles\n", latency);
            break;
        }
        case Opcode::HALT: {
            running_ = false;
            break;
        }
        case Opcode::NOP:
        default:
            // treat unknown as NOP or throw; I recommend throw while developing
            // throw std::runtime_error("Unknown opcode");
            break;
    }
}


int32_t CPU::mem_word(uint32_t addr) const {
    if (addr >= memory_.size()) throw std::out_of_range("mem_word: OOB");
    return memory_[addr];
}

void CPU::enforce_x0() {
    regs_[0] = 0;
}

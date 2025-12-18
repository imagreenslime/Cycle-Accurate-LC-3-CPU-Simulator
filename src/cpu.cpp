#include "cpu.hpp"
#include "cache.hpp"
#include <iostream>
#include <stdexcept>

CPU::CPU(std::vector<Instruction> program)
    : program_(std::move(program)),
      memory_(1 << 20, 0), // 1M words
      cache_(memory_.data())
{}

void CPU::run(int max_steps) {
    printf("loading run\n");
    int steps = 0;
    running_ = true;

    while (running_) {
        if (steps++ >= max_steps) {
            throw std::runtime_error("Max steps exceeded (possible infinite loop)");
        }
        step();
    }

    printf("finished steps\n\n");
}

void CPU::step() {
    // handle branch
    if (branch_taken_) {
        printf("Branch taken\n");
        pc_ = branch_target_;

        // flush younger instruction
        if_id_.valid = false;
        branch_taken_ = false;
    }

    // execute
    if (id_ex_.valid) {
        execute(id_ex_.instr);
        id_ex_.valid = false;
    }

    // decode
    bool stall = false;

    if (if_id_.valid) {
        const Instruction& ins = if_id_.instr;

        // Check dependency with instruction currently in EX
        if (id_ex_.valid) {
            if (has_dependency(ins.rs1, id_ex_.instr) || has_dependency(ins.rs2, id_ex_.instr)) {
                stall = true;
                printf("Stalling due to dependency with EX stage\n");
            }
        }
    }

    if (!stall && if_id_.valid && !id_ex_.valid) {
        id_ex_.instr = if_id_.instr;
        id_ex_.valid = true;
        if_id_.valid = false;
    }

    // fetch
    if (!if_id_.valid) {
        if_id_.instr = fetch();
        if_id_.valid = true;
    }

    enforce_x0();
}


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

        case Opcode::BEQ: {
            if (regs_[instr.rs1] == regs_[instr.rs2]) {
                branch_taken_ = true;
                branch_target_ = instr.pc + instr.imm;
            }
            break;
        }

        case Opcode::JAL: {
            regs_[instr.rd] = instr.pc; // pc already incremented in fetch
            branch_taken_ = true;
            branch_target_ = instr.pc + instr.imm;
            break;
        }

        case Opcode::HALT: {
            running_ = false;
            break;
        }
        case Opcode::NOP:
        default:
            // treat unknown as NOP or throw
            break;
    }
}

bool CPU::writes_rd(const Instruction& instr) {
    switch (instr.op) {
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::ADDI:
        case Opcode::LOAD:
        case Opcode::JAL:
            return true;
        default:
            return false;
    }
}

bool CPU::has_dependency(uint8_t r, const Instruction& older) {
    if (r == 0) return false;          // x0 is always safe
    if (!writes_rd(older)) return false;
    return older.rd == r;
}



int32_t CPU::mem_word(uint32_t addr) const {
    if (addr >= memory_.size()) throw std::out_of_range("mem_word: OOB");
    return memory_[addr];
}

void CPU::enforce_x0() {
    regs_[0] = 0;
}

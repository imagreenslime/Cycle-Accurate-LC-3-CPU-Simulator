#include "cpu.hpp"
#include "cache.hpp"
#include <iostream>
#include <stdexcept>

CPU::CPU(std::vector<Instruction> program)
    : program_(std::move(program)),
      memory_(1 << 20, 0), // 1M words
      cycle_(0),
      instr_count_(0),
      total_cycles_(0),
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

    printf("Instructions: %lu\n", instr_count_);
    printf("Cycles: %lu\n", total_cycles_);
    printf("CPI: %.2f\n", (double)total_cycles_ / instr_count_);
}

void CPU::step() {
    printf("\n=== Cycle %lu ===\n", cycle_);

    print_instr("IF",  if_id_.instr);
    print_instr("ID",  id_ex_.instr);
    print_instr("EX", id_ex_.instr.valid ? id_ex_.instr : Instruction{});
    
    // handle branch
    if (branch_taken_) {
        printf("FLUSH | branch taken → PC=%d\n", branch_target_);
        pc_ = branch_target_;
        if_id_.valid = false;
        id_ex_.valid = false;
        branch_taken_ = false;

        flush_count_++;
    }

    // hazard detection for loads
    bool stall = false;

    if (if_id_.valid && id_ex_.valid && id_ex_.instr.op == Opcode::LOAD) {
        const Instruction& younger = if_id_.instr;
        if (has_dependency(younger.rs1, id_ex_.instr) || has_dependency(younger.rs2, id_ex_.instr)) {
            stall = true;
            printf("STALL | load-use on r%d\n", id_ex_.instr.rd);
            stall_count_++;
        }
    }

    // execute
    if (id_ex_.valid) {
        execute(id_ex_.instr);
        instr_count_++;
        id_ex_.valid = false;   // instruction leaves pipeline
    }

    // decode
    if (!stall && if_id_.valid && !id_ex_.valid) {
        id_ex_.instr = if_id_.instr;
        id_ex_.valid = true;
        if_id_.valid = false;
    }

    // fetch
    if (!stall && !if_id_.valid) {
        if_id_.instr = fetch();
        if_id_.valid = if_id_.instr.valid;
    }

    cycle_++;           // <-- THIS DEFINES A CLOCK EDGE
    total_cycles_++; 
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
        case Opcode::BEQ:
            return true;
        default:
            return false;
    }
}

bool CPU::has_dependency(uint8_t r, const Instruction& older) {
    if (r == 0) return false;              // x0 safe
    if (!older.valid) return false;
    if (!writes_rd(older)) return false;
    return older.rd == r; // does old load affect current function?
}

int32_t CPU::mem_word(uint32_t addr) const {
    if (addr >= memory_.size()) throw std::out_of_range("mem_word: OOB");
    return memory_[addr];
}

const char* opcode_to_str(Opcode op) {
    switch (op) {
        case Opcode::ADD: return "ADD";
        case Opcode::ADDI: return "ADDI";
        case Opcode::SUB: return "SUB";
        case Opcode::LOAD: return "LD";
        case Opcode::STORE: return "ST";
        case Opcode::BEQ: return "BEQ";
        case Opcode::NOP: return "NOP";
        case Opcode::HALT: return "HALT";
        default: return "UNK";
    }
}

void CPU::print_instr(const char* stage, const Instruction& instr) {
    if (!instr.valid) {
        printf("  %-5s | ----\n", stage);
        return;
    }

    printf(
        "  %-5s | PC=%3d OP=%-3s r%d r%d r%d\n",
        stage,
        instr.pc,
        opcode_to_str(instr.op),
        instr.rd,
        instr.rs1,
        instr.rs2
    );
}

void CPU::enforce_x0() {
    regs_[0] = 0;
}

#pragma once
#include <cstdint>

enum class Opcode : uint8_t {
    // ALU
    ADD,
    SUB,
    ADDI,

    // Memory
    LOAD,   // rd = M[rs1 + imm]
    STORE,  // M[rs1 + imm] = rs2

    // Control
    BEQ,    // if (rs1 == rs2) pc += imm
    JAL,    // rd = pc + 1; pc += imm

    // System
    NOP,
    HALT
};

struct Instruction {
    Opcode op = Opcode::NOP;

    // Registers
    uint8_t rd  = 0;
    uint8_t rs1 = 0;
    uint8_t rs2 = 0;

    // Immediate 
    int32_t imm = 0;

    // for debugging
    int pc = -1;
    bool valid = false;
};

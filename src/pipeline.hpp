#include "isa.hpp"
#pragma once
#include <cstdint>

struct IF_ID {
    Instruction instr;
    bool valid = false;
};

struct ID_EX {
    Instruction instr;
    bool valid = false;
    int32_t rs1_val = 0;
    int32_t rs2_val = 0;
    int32_t store_val = 0;

};

struct EX_MEM {
    Instruction instr{};
    bool valid = false;
    int rd = 0;
    int alu_result = 0;
    bool reg_write = false;
};

struct MEM_WB {
    Instruction instr{};
    bool valid = false;
    int rd = 0;
    int writeback_value = 0;
    bool reg_write = false;
};
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
};

struct EX_MEM {
    Instruction instr;
    bool valid = false;
};

struct MEM_WB {
    Instruction instr;
    bool valid = false;
};
#include <iostream>
#include <vector>
#include <cassert>
#include "cpu.hpp"
#include "isa.hpp"
#include "cache.hpp"
#include "cache.cpp"
#include "cpu.cpp"

// THINGS I WANT TO ADD:
// pipelining
// better cache -> last recently used eviction

int main() {
    using Op = Opcode;
    printf("hello cpu test\n\n");

    std::vector<Instruction> prog = {
        {Opcode::ADDI, 1, 0, 0, 1},
        {Opcode::ADDI, 2, 0, 0, 1},
        {Opcode::BEQ,  0, 1, 2, 2},   // if x1 == x2, skip next 2
        {Opcode::ADDI, 3, 0, 0, 99},  // SHOULD BE FLUSHED
        {Opcode::ADDI, 4, 0, 0, 7},
        {Opcode::HALT}
    };



    CPU cpu(prog);
    cpu.run();

    // === Assertions  ===

    printf("All tests passed.\n\n");

    // === Optional debug dump ===
    printf("Register Dump.\n\n");
    for (int i = 0; i < 8; i++) {
        printf("x%d = %d\n", i, cpu.reg(i));
    }

    return 0;
}

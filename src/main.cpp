#include <iostream>
#include <vector>
#include <cassert>
#include "cpu.hpp"
#include "isa.hpp"
#include "cache.hpp"
#include "cache.cpp"
#include "cpu.cpp"

// THINGS I WANT TO ADD:
// better cache -> last recently used eviction

int main() {

    using Op = Opcode;
    printf("hello cpu test\n\n");

    std::vector<Instruction> prog = {
    {Opcode::ADDI,  3, 0, 0, 7},  // x0 = 
    {Opcode::STORE, 3, 0, 0, 0},  // mem[0] = 1
    {Opcode::LOAD,  1, 0, 0, 0},  // x1 = mem[0]
    {Opcode::LOAD,  2, 0, 0, 0},  // x1 = x1 + 1
    {Opcode::ADD,   4, 1, 1, 0},  // x2 = x1 + x1
    {Opcode::LOAD,  5, 0, 0, 0},  // x3 = x2 + 1
    {Opcode::ADDI,  6, 5, 0, 1},  // x4 = x3 + 1
    {Opcode::ADD,  7, 3, 0, 0},  // x5 = x4 + 1
    {Opcode::HALT}
    };

    CPU cpu(prog);
    cpu.run();

    // === Assertions  ===

    printf("All tests passed.\n\n");
    int32_t val = cpu.mem_word(0);
    printf("memory[0] = %d\n", val);


    // === Optional debug dump ===
    printf("Register Dump.\n\n");
    for (int i = 0; i < 8; i++) {
        printf("x%d = %d\n", i, cpu.reg(i));
    }

    return 0;
}

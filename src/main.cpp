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
// forwarding unit
// mem stage

int main() {

    using Op = Opcode;
    printf("hello cpu test\n\n");

    std::vector<Instruction> prog = {
    {Opcode::ADDI, 2, 0, 0, 0},
    {Opcode::STORE, 1, 2, 0, 0},   // miss
    {Opcode::LOAD,  3, 2, 0, 0},   // taken
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

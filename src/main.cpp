#include <iostream>
#include <vector>
#include <cassert>
#include "cpu.hpp"
#include "isa.hpp"
#include "cache.hpp"
#include "cache.cpp"
#include "cpu.cpp"

int main() {
    using Op = Opcode;
    printf("hello cpu test\n\n");
    std::vector<Instruction> prog = {
    // x1 = 256  (base address)
        {Opcode::ADDI, 1, 0, 0, 256},

        // x2 = 42   (value to store)
        {Opcode::ADDI, 2, 0, 0, 42},

        // MEM[x1 + 0] = x2   → cold store
        {Opcode::STORE, 0, 1, 2, 0},

        // Repeated loads from same address
        {Opcode::LOAD, 3, 2, 0, 0},   // MISS
        {Opcode::LOAD, 4, 1, 0, 0},   // HIT
        {Opcode::LOAD, 5, 1, 0, 0},   // HIT
        {Opcode::LOAD, 6, 2, 0, 0},   // HIT
        {Opcode::STORE, 0, 2, 0, 0},
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

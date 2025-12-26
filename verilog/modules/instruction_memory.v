module instruction_memory (
    input wire [31:0] address,
    output wire [31:0] instruction
);

    reg [31:0] memory [0:255];
    initial begin
        // Initialize the memory with some instructions
        memory[0] = 32'h22000001; // add r1 => 1
        memory[1] = 32'h22000017; // add r2 => 23
        memory[2] = 32'h40020003; // st r2 -> 3
        memory[3] = 32'h34000003; // ld r3 <= 3
        memory[4] = 32'h50000002; // beq r0, r0, +2
        memory[5] = 32'h34000003; // ld r4 <= 4
        memory[6] = 32'h25200017; // add r5 => 23
        memory[7] = 32'h60000004; //HALT
        // Add more instructions as needed
        
    end

    assign instruction = memory[address];
endmodule
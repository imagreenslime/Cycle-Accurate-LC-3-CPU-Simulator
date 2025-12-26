`include "modules/pc.v"
`include "modules/instruction_memory.v"

module instruction_fetch (
    input wire clk,
    input wire reset,
    input wire load_en,
    input wire [31:0] pc_next,
    output wire [31:0] current_instruction,
    output wire [31:0] current_pc
);
    wire [31:0] pc_value;
    wire [31:0] instruction;
    pc program_counter (
        .clk(clk),
        .reset(reset),
        .load_en(load_en),
        .d(pc_next),
        .q(current_pc)
    );
    instruction_memory inst_mem (
        .address(current_pc),
        .instruction(current_instruction)
    );
endmodule
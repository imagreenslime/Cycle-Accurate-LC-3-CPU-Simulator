`include "modules/instruction_fetch.v"
`include "modules/isa_decoder.v"
`include "modules/register_file.v"
`include "modules/data_memory.v"
`include "modules/execute.v"

module cpu (
    input wire clk,
    input wire reset
);

    // ======== Internal Wires ========
    wire [31:0] instruction;
    wire [31:0] pc_value;

    // Outputs from execute
    wire [3:0] opcode, rs1, rs2, rd;
    wire [15:0] imm;
    wire [31:0] rs1_val, rs2_val;
    wire [31:0] rd_value;
    wire [31:0] mem_data_out;
    wire [31:0] mem_data_in;
    wire [31:0] mem_addr;

    wire reg_write_en;
    wire mem_read_en, mem_write_en;
    wire branch_taken;
    wire [15:0] branch_target;
    wire halt;
    // Pipelining
    
    // from fetch
    wire [31:0] if_instr_w;
    wire [31:0] if_pc_w;
    reg [31:0] if_id_instr;
    reg [31:0] if_id_pc;

    // from decode
    wire [3:0]  id_ex_opcode_w;
    wire [3:0]  id_ex_rd_w;
    wire [3:0]  id_ex_rs1_w;
    wire [3:0]  id_ex_rs2_w;
    wire [15:0] id_ex_imm_w;
    wire [31:0] id_ex_rs1_val_w;
    wire [31:0] id_ex_rs2_val_w;
    
    reg [3:0]  id_ex_opcode;
    reg [3:0]  id_ex_rd;
    reg [3:0]  id_ex_rs1;
    reg [3:0]  id_ex_rs2;
    reg [15:0] id_ex_imm;
    reg [31:0] id_ex_rs1_val;
    reg [31:0] id_ex_rs2_val;
    reg [31:0] id_ex_pc;
    
    // from execute

    wire ex_mem_read_en_w;
    wire ex_mem_write_en_w;
    wire [31:0] ex_mem_addr_w;
    wire [31:0] ex_mem_data_out_w;

    wire ex_mem_branch_taken_w;
    wire [15:0] ex_mem_branch_target_w;
    wire ex_mem_halt_w;

    reg [31:0] ex_mem_pc;

    reg ex_mem_read_en;
    reg ex_mem_write_en;
    reg [31:0] ex_mem_addr;
    reg [31:0] ex_mem_data_out;

    reg ex_mem_branch_taken;
    reg [15:0] ex_mem_branch_target;
    reg ex_mem_halt;

    // Register store
    wire [31:0] mem_wb_rd_value_w;
    wire mem_wb_reg_write_en_w;

    reg [3:0] mem_wb_rd;
    reg [31:0] mem_wb_rd_value;
    reg mem_wb_reg_write_en;

    // PC logic for fetch
    wire pc_load_en = branch_taken;
    wire [31:0] pc_next = branch_taken ? {32'd0, branch_target} : (pc_value + 1);
    
    // ======== FETCH ========
    instruction_fetch fetch (
        .clk(clk),
        .reset(reset),
        .load_en(pc_load_en),
        .pc_next(pc_next),
        .current_instruction(if_instr_w),
        .current_pc(if_pc_w)
    );

    always @(posedge clk) begin
        if (reset) begin
            // for exe now
            if_id_instr <= 32'b0;
            if_id_pc    <= 32'b0;
        end else begin
            if_id_instr <= if_instr_w;
            if_id_pc    <= if_pc_w;
        end
    end

    // ======== DECODE ========
    isa_decoder decoder (
        .instruction(if_id_instr),
        .opcode(id_ex_opcode_w),
        .rs1(id_ex_rs1_w),
        .rs2(id_ex_rs2_w),
        .rd(id_ex_rd_w),
        .imm(id_ex_imm_w)
    );

    // ======== REGISTER FILE ========
    register_file regfile (
        .clk(clk),
        .rs1(id_ex_rs1_w),
        .rs2(id_ex_rs2_w),
        .rd(mem_wb_rd),
        .write_data(mem_wb_rd_value),
        .reg_write(mem_wb_reg_write_en),
        .rs1_data(id_ex_rs1_val_w),
        .rs2_data(id_ex_rs2_val_w)
    );

    
    always @(posedge clk) begin
        if (reset) begin
            id_ex_opcode   <= 4'b0;
            id_ex_rs1      <= 4'b0;
            id_ex_rs2      <= 4'b0;
            id_ex_rd       <= 4'b0;
            id_ex_imm      <= 16'b0;
            id_ex_rs1_val  <= 32'b0;
            id_ex_rs2_val  <= 32'b0;
            id_ex_pc       <= 32'b0;
        end else begin
            id_ex_opcode   <= id_ex_opcode_w;
            id_ex_rs1      <= id_ex_rs1_w;
            id_ex_rs2      <= id_ex_rs2_w;
            id_ex_rd       <= id_ex_rd_w;
            id_ex_imm      <= id_ex_imm_w;
            id_ex_rs1_val  <= id_ex_rs1_val_w;
            id_ex_rs2_val  <= id_ex_rs2_val_w;
            id_ex_pc       <= if_id_pc;          // or id_ex_pc_w if you want a wire
        end
    end


    
    // ======== EXECUTE ========
    execute exec (
        .opcode(id_ex_opcode),
        .rs1_val(id_ex_rs1_val),
        .rs2_val(id_ex_rs2_val),
        .rd(id_ex_rd),
        .imm(id_ex_imm),
        .pc(id_ex_pc[15:0]),
        .mem_data_in(mem_data_in),

        .rd_value(mem_wb_rd_value_w),
        .reg_write_en(mem_wb_reg_write_en_w),

        .mem_read_en(ex_mem_read_en_w),
        .mem_write_en(ex_mem_write_en_w),
        .mem_addr(ex_mem_addr_w),
        .mem_data_out(ex_mem_data_out_w),

        .branch_taken(ex_mem_branch_taken_w),
        .branch_target(ex_mem_branch_target_w),
        .halt(ex_mem_halt_w)
    );
    
    always @(posedge clk) begin
        if (reset) begin
            ex_mem_pc <= 32'd0;
            mem_wb_rd_value <= 32'd0;
            mem_wb_rd <= 4'd0;
            mem_wb_reg_write_en <= 1'd0;

            ex_mem_read_en <= 1'd0;
            ex_mem_write_en <= 1'd0;
            ex_mem_addr <= 32'd0;
            ex_mem_data_out <= 32'd0;

            ex_mem_branch_taken <= 1'd0;
            ex_mem_branch_target <= 16'd0;
            ex_mem_halt <= 1'd0;
        end else begin
            ex_mem_pc <= id_ex_pc;
            mem_wb_rd_value <= mem_wb_rd_value_w;
            mem_wb_rd <= id_ex_rd;
            mem_wb_reg_write_en <= mem_wb_reg_write_en_w;

            ex_mem_read_en <= ex_mem_read_en_w;
            ex_mem_write_en <= ex_mem_write_en_w;
            ex_mem_addr <= ex_mem_addr_w;
            ex_mem_data_out <= ex_mem_data_out_w;

            ex_mem_branch_taken <= ex_mem_branch_taken_w;
            ex_mem_branch_target <= ex_mem_branch_target_w;
            ex_mem_halt <= ex_mem_halt_w;
        end
    end

    // ======== DATA MEMORY ========
    data_memory dmem (
        .clk(clk),
        .mem_read(ex_mem_read_en),
        .mem_write(ex_mem_write_en),
        .address(ex_mem_addr),
        .write_data(ex_mem_data_out),
        .read_data(mem_data_in)
    );


    always @(posedge clk) begin
        $display("============== CYCLE @ %0t ns ==============", $time);
        $display("Fetch Phase: pc: %0d, instr: %0d", if_id_pc, if_id_instr);
        $display("Decode Phase: pc: %0d, op: %0d, rd: %0d, rs1: %0d, rs2: %0d, imm: %0d, rs1_val: %0d, rs2_val: %0d",id_ex_pc, id_ex_opcode, id_ex_rd, id_ex_rs1, id_ex_rs2, id_ex_imm, id_ex_rs1_val, id_ex_rs2_val);
        $display("Execute Phase: pc: %0d, rd: %0d, rd val: %0d, mem_addr: %0d, branch_taken: %0d, branch_targ: %0d, data_out: %0d, halt: %0d", ex_mem_pc, mem_wb_rd, mem_wb_rd_value, ex_mem_addr, ex_mem_branch_taken, ex_mem_branch_target, ex_mem_data_out, ex_mem_halt);
        $display("Execute Phase: reg write: %0d, read enable: %0d, write enable: %0d", mem_wb_reg_write_en_w, ex_mem_read_en, ex_mem_write_en);
        if (ex_mem_halt) begin
            $display("HALT instruction encountered at PC=%0d. Halting simulation.", pc_value);
            regfile.display_all();
            $finish;
        end
    end

endmodule

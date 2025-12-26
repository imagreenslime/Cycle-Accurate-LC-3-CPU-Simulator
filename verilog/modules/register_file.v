module register_file (
    input wire clk,
    input wire [3:0] rs1, 
    input wire [3:0] rs2,      
    input wire [3:0] rd,     
    input wire [31:0] write_data, // data to write
    input wire reg_write,         // write enable

    output wire [31:0] rs1_data,  // data from rs1
    output wire [31:0] rs2_data   // data from rs2
);

    // 16 x 32-bit registers
    
    reg [31:0] registers [15:0];

    integer i;
    initial begin

        for (i = 0; i < 16; i = i + 1) begin
            registers[i] = 32'd0; // or any value like 32'd1, 32'hDEADBEEF, etc.
        end
        // Optional: show confirmation in simulation
        $display("Register file initialized.");
    end

    task display_all;
        integer j;
        begin
            $display("=== Register File Contents ===");
            for (j = 0; j < 16; j = j + 1) begin
                $display("r%0d = %0d", j, registers[j]);
            end
        end
    endtask

    // Read ports (asynchronous)
    assign rs1_data = registers[rs1]; 
    assign rs2_data = registers[rs2]; 

    // Write port (synchronous)
    always @(posedge clk) begin
        if (reg_write) begin
            registers[rd] <= write_data;
        end
    end
endmodule   

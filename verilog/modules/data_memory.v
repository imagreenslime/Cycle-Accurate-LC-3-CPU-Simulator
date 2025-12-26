module data_memory (
    input wire clk,
    input wire mem_read,
    input wire mem_write,
    input wire [31:0] address,
    input wire [31:0] write_data,
    output reg [31:0] read_data
);

    // 1K words = 4KB memory
    reg [31:0] memory [0:1023];

    // READ logic (combinational read)
    integer i;
    initial begin
        for (i = 0; i < 1024; i = i + 1) begin
            memory[i] = 32'd0; // or any value like 32'd1, 32'hDEADBEEF, etc.
        end
        // Optional: show confirmation in simulation
        $display("Data Memory initialized.");
    end
    always @(*) begin
        //$display("Data Memory Access - Read: %b, Write: %b, Address: %0d, Write Data: %0d", mem_read, mem_write, address, write_data);  
        if (mem_read) begin
            read_data = memory[address]; // word-aligned
        end else begin
            read_data = 32'd0;
        end
    end

    // WRITE logic (synchronous)
    always @(posedge clk) begin
        if (mem_write) begin
            memory[address] <= write_data; // word-aligned
        end
    end

endmodule

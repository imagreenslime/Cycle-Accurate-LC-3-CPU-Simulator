`timescale 1ns/1ps
`include "./cpu.v"
module test_cpu;
    reg clk = 0;
    reg reset = 1;

    cpu DUT (
        .clk(clk),
        .reset(reset)
    );

    always #5 clk = ~clk;

    initial begin
        $dumpfile("cpu.vcd");
        $dumpvars(0, test_cpu);
        #10;
        reset = 0;

        #2000; // Let CPU run for some cycles
        $finish;
    end
endmodule

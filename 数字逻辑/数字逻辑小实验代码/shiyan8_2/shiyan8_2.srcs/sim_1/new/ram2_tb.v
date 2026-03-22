`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/18 23:55:16
// Design Name: 
// Module Name: ram2_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ram2_tb;
    reg clk; 
    reg ena;
    reg wena; 
    reg [4:0] addr;
    wire [31:0] data;//inout必须是wire型
    ram2 inst(
        .clk(clk),
        .ena(ena),
        .wena(wena),
        .addr(addr),
        .data(data)
        );
    integer i;
            initial
            begin
                clk=0;
                for(i=0;i<500;i=i+1)
                begin
                    #5;
                    clk=~clk;
                end
            end
            initial
            begin
                ena=0;
                #20 ena=1;
            end
            initial
            begin
                wena=0;
                #10 wena=1;
                #30 wena=0;
            end
            initial
            begin
               force data=32'habcdef12; //fprce强行改变值，观察读操作
               #30 force data=32'h12345678;
               #10;
            end
            initial
            begin
                addr=5'b01010;
                #30 addr=5'b10101;
                #10 addr=5'b01010;
                #10 addr=5'b10101;
                #10;
            end
endmodule

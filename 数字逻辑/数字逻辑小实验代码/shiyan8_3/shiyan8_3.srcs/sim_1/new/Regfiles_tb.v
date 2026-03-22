`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/19 01:01:52
// Design Name: 
// Module Name: Regfiles_tb
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


module Regfiles_tb;
    reg clk;
    reg rst;
    reg we;
    reg [4:0] raddr1;
    reg [4:0] raddr2;
    reg [4:0] waddr;
    reg [31:0] wdata;
    wire [31:0] rdata1;
    wire [31:0] rdata2;
 
    Regfiles uut (
        .clk(clk),
        .rst(rst),
        .we(we),
        .raddr1(raddr1),
        .raddr2(raddr2),
        .waddr(waddr),
        .wdata(wdata),
        .rdata1(rdata1),
        .rdata2(rdata2)
    );
 
    initial begin
        clk = 0;
        forever #5 clk = ~clk; // T=10;
    end
 
    initial begin
        rst = 1; //高电平复位
        we = 0;
        raddr1 = 0;
        raddr2 = 0;
        waddr = 0;
        wdata = 0;
 
        #10;
        rst = 0; 
 
        // 写操作
        #10;
        we = 1;
        waddr = 5'b00010;
        wdata = 32'h12345678; // 写数据
        #10;
        we = 0; // 停止写操作
        wdata=32'h00000000;
        // 读操作
        #5;
        raddr1 = 5'b00010; // 读取\
        #10;
        //观察dataout
        
        #10 raddr2 = 5'b00100; 
        #10; 
 
    end
 
endmodule

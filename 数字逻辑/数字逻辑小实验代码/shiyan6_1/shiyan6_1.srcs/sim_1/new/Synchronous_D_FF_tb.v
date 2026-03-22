`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/28 18:33:36
// Design Name: 
// Module Name: Synchronous_D_FF_tb
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


module Synchronous_D_FF_tb;
  reg CLK;  
  reg D;  
  reg RST_n;  
  wire Q1;  
  wire Q2;  

Synchronous_D_FF uut (  
  .CLK(CLK),  
  .D(D),  
  .RST_n(RST_n),  
  .Q1(Q1),  
  .Q2(Q2)  
);  

// 时钟生成 
initial begin  
  CLK = 0;  
  forever #5 CLK = ~CLK; // 10ns时钟周期  
end  

// 测试  
initial begin  
  // 初始化信号  
  RST_n = 0;  
  D = 0;  

  #15;  
 
  RST_n = 1;  

  // 测试序列  
  #10 D = 1;  
  #10 D = 0; 
  #5 RST_n = 0; 
  #10 D = 1;  
  #5 RST_n = 1; 
  #10 D = 0; 
  #10 D = 1;   
  end
  
endmodule

`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/30 23:39:22
// Design Name: 
// Module Name: de_selector14_teb
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


module de_selector14_teb;  
  
// 输入和输出信号的声明  
    reg iC;  
    reg iS1;  
    reg iS0;  
    wire oZ0;  
    wire oZ1;  
    wire oZ2;  
    wire oZ3;  
  
// 实例化数据分配器模块  
    de_selector14 
    uut (.iC(iC),.iS1(iS1),.iS0(iS0),.oZ0(oZ0),.oZ1(oZ1),.oZ2(oZ2),.oZ3(oZ3));  
  
// 初始化块，用于设置测试序列  
initial begin  
    // 初始化输入信号  
    iC = 0;  
    iS1 = 0;  
    iS0 = 0;  
    // 测试序列  
    // 测试 iC = 0 时的情况  
    #10
    iC = 0; iS1 = 0; iS0 = 0;
    #10   
    iC = 0; iS1 = 0; iS0 = 1;
    #10  
    iC = 0; iS1 = 1; iS0 = 0;
    #10
    iC = 0; iS1 = 1; iS0 = 1;

    end  
 
endmodule

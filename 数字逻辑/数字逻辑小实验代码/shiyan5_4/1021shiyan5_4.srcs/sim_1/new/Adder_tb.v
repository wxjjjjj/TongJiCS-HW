`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/21 20:30:27
// Design Name: 
// Module Name: Adder_tb
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


module Adder_tb;

    reg [7:0] iData_a;  
    reg [7:0] iData_b;  
    reg iC;  
  
    wire [7:0] oData;  
    wire oData_C;  
  
    Adder uut (  
        .iData_a(iData_a),  
        .iData_b(iData_b),  
        .iC(iC),  
        .oData(oData),  
        .oData_C(oData_C)  
    );  
  
    initial begin   
     
        iData_a = 8'h00; iData_b = 8'h00; iC = 0; #10; // 0 + 0 + 0 = 0, 进位0  
        iData_a = 8'h01; iData_b = 8'h01; iC = 0; #10; // 1 + 1 + 0 = 2, 进位0  
        iData_a = 8'h7F; iData_b = 8'h01; iC = 0; #10; // 127 + 1 + 0 = 128, 进位1  
        iData_a = 8'hFF; iData_b = 8'h01; iC = 0; #10; // 255 + 1 + 0 = 0 (8位溢出), 进位1  
        iData_a = 8'hFF; iData_b = 8'hFF; iC = 0; #10; //1111 1110
   
    end  
  
endmodule

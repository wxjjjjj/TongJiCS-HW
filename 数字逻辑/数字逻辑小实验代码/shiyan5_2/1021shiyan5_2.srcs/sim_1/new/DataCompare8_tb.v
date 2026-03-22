`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/21 19:02:39
// Design Name: 
// Module Name: DataCompare8_tb
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


module DataCompare8_tb;
    reg [7:0] iData_a;  
    reg [7:0] iData_b;  
    wire [2:0] oData;  
  
    DataCompare8 uut (  
        .iData_a(iData_a),  
        .iData_b(iData_b),  
        .oData(oData)  
    );  
  
    initial begin  
        // 测试1: iData_a 大于 iData_b 
        iData_a = 8'b0101_0100; iData_b = 8'b0011_0010; 
         #10;  
        // 测试2: iData_a 小于 iData_b  
        iData_a = 8'b0011_0101; iData_b = 8'b0101_0111;
         #10;  
        // 测试3: iData_a 等于 iData_b  
        iData_a = 8'b0101_0000; iData_b = 8'b0101_0000; 
         #10;  
 
    end  

endmodule

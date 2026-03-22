`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/21 16:12:54
// Design Name: 
// Module Name: DataComparer4_tb
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


module DataCompare4_tb;  

    reg [3:0] iData_a;  
    reg [3:0] iData_b;  
    reg [2:0] iData;  
    wire [2:0] oData;  
  
    DataCompare4 uut (  
        .iData_a(iData_a),  
        .iData_b(iData_b),  
        .iData(iData),  
        .oData(oData)  
    );  
  
    initial begin  
        // 测试1: iData_a 大于 iData_b 
        iData_a = 4'b0101; iData_b = 4'b0011; iData = 3'b000;
         #10;  
        // 测试2: iData_a 小于 iData_b  
        iData_a = 4'b0011; iData_b = 4'b0101; iData = 3'b000;
         #10;  
        // 测试3: iData_a 等于 iData_b  
        iData_a = 4'b0101; iData_b = 4'b0101; iData = 3'b100;
         #10;  
        // 测试4: 使用iData的特殊条件（假设这里表示某种特殊情况下的相等）  
        iData_a = 4'b0101; iData_b = 4'b0101; iData = 3'b010;
         #10;  
        // 测试5: 另一个iData的特殊条件（假设这里表示某种特殊情况下的iData_a小于iData_b）  
        iData_a = 4'b0101; iData_b = 4'b0101; iData = 3'b001;
         #10;  
  
    end  
  
endmodule

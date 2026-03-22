`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/12 19:54:20
// Design Name: 
// Module Name: display7_tb
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


module display7_tb; 
     
    reg [3:0] iData;  
    wire [6:0] oData;  
  
    // 实例化 display7 模块  
    display7 uut (  
        .iData(iData),  
        .oData(oData)  
    );  
  
  
    initial begin  
         
  
        // 测试不同的输入值  
        iData = 4'b0000; #10; // 等待10个时间单位  
        iData = 4'b0001; #10;  
        iData = 4'b0010; #10;  
        iData = 4'b0011; #10;  
        iData = 4'b0100; #10;  
        iData = 4'b0101; #10;  
        iData = 4'b0110; #10;  
        iData = 4'b0111; #10;  
        iData = 4'b1000; #10;  
        iData = 4'b1001; #10;  

    end  
endmodule

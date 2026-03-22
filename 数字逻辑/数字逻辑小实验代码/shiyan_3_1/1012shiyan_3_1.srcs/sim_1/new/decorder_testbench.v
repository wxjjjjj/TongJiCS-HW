`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/12 16:50:29
// Design Name: 
// Module Name: decorder_testbench
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


module decorder_testbench;
    reg [2:0] iData;      
    reg [1:0] iEna;          
    wire [7:0] oData;      
  
    // 实例化译码器模块  
    decoder uut (  
        .iData(iData),  
        .iEna(iEna),  
        .oData(oData)  
    );  
  
      
    initial 
    begin  
        
        
        iEna = 2'b10;  
        iData = 3'b000; #10; 
        iData = 3'b001; #10; 
        iData = 3'b010; #10;   
        iData = 3'b011; #10;   
        iData = 3'b100; #10;   
        iData = 3'b101; #10;   
        iData = 3'b110; #10;   
        iData = 3'b111; #10;   
  
        // 使能信号为其他值时测试（应该输出全0）  
        iEna = 2'b00; #10;   
        iData = 3'b000; #10;  
        iData = 3'b111; #10;  
  
        iEna = 2'b01; #10;   
        iData = 3'b000; #10;  
        iData = 3'b111; #10;  
  
        iEna = 2'b11; #10;   
        iData = 3'b000; #10;  
        iData = 3'b111; #10;  
         
    end  
endmodule

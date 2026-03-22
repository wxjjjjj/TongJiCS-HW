`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/12 20:13:53
// Design Name: 
// Module Name: encoder_tb
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


module encoder_tb;
    reg [7:0] iData;           
    wire [2:0] oData;      
  
    // ÊµÀý»¯ÒëÂëÆ÷Ä£¿é  
    encoder83 uut (  
        .iData(iData),    
        .oData(oData)  
    );  
  
      
    initial 
    begin  
     
        iData = 8'b10000000; #10;
        iData = 8'b01000000; #10;
        iData = 8'b00100000; #10;
        iData = 8'b00010000; #10;
        iData = 8'b00001000; #10;
        iData = 8'b00000100; #10;
        iData = 8'b00000010; #10;
        iData = 8'b00000001; #10;
         
    end  

endmodule

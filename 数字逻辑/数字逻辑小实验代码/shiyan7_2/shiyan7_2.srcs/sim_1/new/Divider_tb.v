`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/04 17:16:55
// Design Name: 
// Module Name: Divider_tb
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


module Divider_tb;
    reg I_CLK;  
    reg rst;  
    wire O_CLK;  
  
    Divider uut (  
        .I_CLK(I_CLK),  
        .rst(rst),  
        .O_CLK(O_CLK)  
    );  
   
    initial begin  
        I_CLK = 0;  
        forever #5 I_CLK = ~I_CLK; //周期为10 
    end  
  
    initial begin  
       
        rst = 0;  
        #380;  
        
        rst=1; //验证同步复位
        #5;   
 
    end  
endmodule

`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/28 18:45:46
// Design Name: 
// Module Name: Asynchronous_D_FF
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


module Asynchronous_D_FF(
    input CLK,
    input RST_n,
    input D,
    output reg Q1,
    output reg Q2
    );
    
    always@(posedge CLK or negedge RST_n)begin
          if(!RST_n)begin //如果上升沿到来时，复位信号为0--有效复位
           Q1<=0;
           Q2<=1;//Q2一定与Q1相反啊啊啊
          end
          else begin
           Q1<=D;
           Q2<=~D;
          end
         end
    
endmodule

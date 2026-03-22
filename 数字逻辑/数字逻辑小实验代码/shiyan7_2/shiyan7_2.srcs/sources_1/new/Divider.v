`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/04 17:03:25
// Design Name: 
// Module Name: Divider
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


module Divider(
    input I_CLK,
    input rst,
    output O_CLK
    );
    
    parameter N=20;         //设计模块中不能用initional初始化！！！！
        reg [31:0]n=32'h00000000;
        reg q=0;
       always @(posedge I_CLK )begin
        if(rst==1)begin
          q<=0;
          n<=32'h00000000;
         end else begin
               if(n==(N/2-1)) begin
                        q<=~q;
                        n<=32'h00000000;
                end else
                        n<=n+1;
          end
       end
       assign O_CLK=q;
       
endmodule

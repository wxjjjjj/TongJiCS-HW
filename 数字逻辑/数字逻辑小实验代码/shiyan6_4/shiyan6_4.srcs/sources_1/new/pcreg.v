`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/28 19:54:43
// Design Name: 
// Module Name: pcreg
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


module pcreg(
    input clk,
    input rst,
    input ena,
    input [31:0] data_in,
    output [31:0] data_out
    );
    genvar i;  
        generate  
            for (i = 0; i < 32; i = i + 1) begin : pc_bits  
                Asynchronous_D_FF u_ff (  
                    .CLK(clk),  
                    .RST_n(rst),
                    .ena(ena),  
                    .D(data_in[i]),  
                    .Q1(data_out[i])  
                );  
            end  
        endgenerate  
endmodule


module Asynchronous_D_FF(
    input CLK,
    input RST_n,
    input D,
    input ena,
    output reg Q1  //q1就是一个输出就够了
    );
    
    always@(posedge CLK or posedge RST_n)begin
          if(RST_n)begin //如果上升沿到来时，复位信号为1--有效复位
           Q1<=0;
          end
          else begin
           if(ena)begin
            Q1<=D;
           end
           else begin
            Q1<=Q1;
           end
          end
     end
endmodule


`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/12 16:35:23
// Design Name: 
// Module Name: decoder
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


module decoder(iData,iEna,oData);
    input [2:0] iData;//{D2,d1,d0}
    input [1:0] iEna;
    output reg[7:0] oData;//{y7...y0}
     always @(*) begin  
           if (iEna == 2'b10) begin  
               // G1为1，G2为0  
               case (iData)  
                   3'b000: oData = 8'b11111110;  
                   3'b001: oData = 8'b11111101;  
                   3'b010: oData = 8'b11111011;  
                   3'b011: oData = 8'b11110111;  
                   3'b100: oData = 8'b11101111;  
                   3'b101: oData = 8'b11011111;  
                   3'b110: oData = 8'b10111111;  
                   3'b111: oData = 8'b01111111;  
                   default: oData = 8'b11111111; // 默认值，虽然实际上不会用到  
               endcase  
           end  
           else
           begin  
           oData=8'b11111111; 
       end 
       end 
endmodule

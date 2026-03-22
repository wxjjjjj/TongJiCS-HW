`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/12 19:41:50
// Design Name: 
// Module Name: display7
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


module display7(iData,oData);
    input [2:0] iData;   //d2,d1,d0
    output reg [6:0] oData;   //g~a
    always @(*) begin  
           case (iData)  
                       3'b000: oData = 7'b1000000; //0 
                       3'b001: oData = 7'b1111001;  //1
                       3'b010: oData = 7'b0100100;//2  
                       3'b011: oData = 7'b0110000;//3  
                       3'b100: oData = 7'b0011001;  //4
                       3'b101: oData = 7'b0010010;  //5
                       3'b110: oData = 7'b0000010;  //6
                       3'b111: oData = 7'b1111000;  //7
                       default: oData = 7'b0000000; 
            endcase  
        end  
endmodule

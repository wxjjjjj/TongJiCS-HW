`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/30 21:07:18
// Design Name: 
// Module Name: selector41
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


module selector41(iC0,iC1,iC2,iC3,iS1,iS0,oZ);
    input [3:0] iC0;
    input [3:0] iC1;
    input [3:0] iC2;
    input [3:0] iC3;
    input iS1;
    input iS0;
    output reg [3:0] oZ;

    always @(*) begin  
        case ({iS1, iS0})  // Combine iS2 and iS1 into a 2-bit selector  
            2'b00: oZ = iC0;  // When iS1=0 and iS2=0, select iC0  
            2'b01: oZ = iC1;  // When iS1=0 and iS0=1, select iC1  
            2'b10: oZ = iC2;  // When iS1=1 and iS0=0, select iC2  
            2'b11: oZ = iC3;  // When iS1=1 and iS0=1, select iC3  
            default: oZ = 4'b0000;  // Default case, though it should never be reached  
        endcase  
    end  
endmodule

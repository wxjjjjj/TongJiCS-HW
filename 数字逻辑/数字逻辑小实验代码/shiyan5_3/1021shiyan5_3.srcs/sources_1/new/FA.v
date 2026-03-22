`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/21 19:31:23
// Design Name: 
// Module Name: FA
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


module FA(
    input iA,
    input iB,
    input iC,
    output  oS,
    output  oC
    );
    wire xor1,and1,and2;
     
    xor (xor1,iA,iB);
    xor (oS,xor1,iC);
    and(and1,iC,xor1);
    and(and2,iA,iB);
    or(oC,and1,and2); 
 
    
endmodule

`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/21 19:56:29
// Design Name: 
// Module Name: FA_tb
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


module FA_tb;
    reg iA;  
    reg iB;  
    reg iC;  
   
    wire oS;  
    wire oC;  
   
    FA uut (  
        .iA(iA),  
        .iB(iB),  
        .iC(iC),  
        .oS(oS),  
        .oC(oC)  
    );  
   
    initial begin  
        
        iA = 0; iB = 0; iC = 0; 
        #10; // 0 + 0 + 0 = 0, 进位0  
        iA = 0; iB = 0; iC = 1; 
        #10; // 0 + 0 + 1 = 1, 进位0  
        iA = 0; iB = 1; iC = 0;
         #10; // 0 + 1 + 0 = 1, 进位0  
        iA = 0; iB = 1; iC = 1;
         #10; // 0 + 1 + 1 = 0, 进位1  
        iA = 1; iB = 0; iC = 0; 
        #10; // 1 + 0 + 0 = 1, 进位0  
        iA = 1; iB = 0; iC = 1;
         #10; // 1 + 0 + 1 = 0, 进位1  
        iA = 1; iB = 1; iC = 0; 
        #10; // 1 + 1 + 0 = 0, 进位1  
        iA = 1; iB = 1; iC = 1;
         #10; // 1 + 1 + 1 = 1, 进位1  
  
    end  
endmodule

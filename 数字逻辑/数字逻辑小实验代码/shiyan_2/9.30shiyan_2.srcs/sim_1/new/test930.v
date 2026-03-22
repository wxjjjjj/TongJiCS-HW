`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/30 22:01:07
// Design Name: 
// Module Name: test930
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


module test930;
    reg [3:0] iC0;  
    reg [3:0] iC1;  
    reg [3:0] iC2;  
    reg [3:0] iC3;  
    reg iS1;  
    reg iS0;  
    wire [3:0] oZ;  
  
    initial begin  
       
        // 应用测试向量  
        iC0 = 4'hA; iC1 = 4'hB; iC2 = 4'hC; iC3 = 4'hD; iS1 = 0; iS0 = 0; #10;  
        iC0 = 4'hA; iC1 = 4'hE; iC2 = 4'hC; iC3 = 4'hD; iS1 = 0; iS0 = 1; #10;  
        iC0 = 4'hA; iC1 = 4'hB; iC2 = 4'hF; iC3 = 4'hD; iS1 = 1; iS0 = 0; #10;  
        iC0 = 4'hA; iC1 = 4'hB; iC2 = 4'hC; iC3 = 4'hG; iS1 = 1; iS0 = 1; #10;  
        
    end  
       
       selector41 
       uut (  
           .iC0(iC0),  
           .iC1(iC1),  
           .iC2(iC2),  
           .iC3(iC3),  
           .iS1(iS1),  
           .iS2(iS0),  
           .oZ(oZ)  
       );
endmodule

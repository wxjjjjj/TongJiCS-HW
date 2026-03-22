`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/30 22:31:54
// Design Name: 
// Module Name: test_930
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


module selector41_tb;
    reg [3:0] iC0;  
    reg [3:0] iC1;  
    reg [3:0] iC2;  
    reg [3:0] iC3;  
    reg iS1;  
    reg iS0;  
    wire [3:0] oZ;  
  
    initial 
    begin  
        iC0=0;iC1=0;iC2=0;iC3=0;
        iS0=0;iS1=0;
        #10
        iC0 = 4'hA; iC1 = 4'hB; iC2 = 4'hC; iC3 = 4'hD; iS1 = 0; iS0 = 0;  
        #10
        iC0 = 4'hE; iC1 = 4'hF; iC2 = 4'hA; iC3 = 4'hB; iS1 = 0; iS0 = 1;
        #10  
        iC0 = 4'hC; iC1 = 4'hD; iC2 = 4'hE; iC3 = 4'hF; iS1 = 1; iS0 = 0;
        #10  
        iC0 = 4'hA; iC1 = 4'hB; iC2 = 4'hC; iC3 = 4'hD; iS1 = 1; iS0 = 1; 
    end  
       
       selector41 
       uut (.iC0(iC0),.iC1(iC1),.iC2(iC2),.iC3(iC3),.iS1(iS1),.iS0(iS0),.oZ(oZ));
endmodule

`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/28 19:30:37
// Design Name: 
// Module Name: JK_FF_tb
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


module JK_FF_tb;
   reg CLK;  
   reg J;  
   reg K;  
   reg RST_n;  
      
   wire Q1;  
   wire Q2;  
     
   JK_FF uut (  
       .CLK(CLK),  
       .J(J),  
       .K(K),  
       .RST_n(RST_n),  
       .Q1(Q1),  
       .Q2(Q2)  
   );  
       
   initial begin  
       CLK = 0;  
       forever #5 CLK = ~CLK;  
   end  
     
   initial begin  
    
       RST_n = 0;  
       J = 0;  
       K = 0;  
          
       #5;  
         
       RST_n = 1;  
         
       #10;  
       J = 1; K = 0;  
       #5;  
       RST_n = 0;
       #5;
       RST_n = 1;
       #5;
        
       J = 0; K = 1;  
       #10;  
         
       J = 0; K = 0;  
       #10;  
         
       J = 1; K = 1;  
       #10;  
           
       J = 1; K = 1;  
       #10;  
          
       J = 0; K = 0;  
       #10;  
       
   end  

endmodule

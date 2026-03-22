`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/12 20:47:08
// Design Name: 
// Module Name: encoder83_pti_tb
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


module encoder83_pti_tb;
  
   reg [7:0] iData;  
   reg iEI;  
   wire [2:0] oData;  
   wire oEO;  
 
    
   encoder83_Pri uut (  
       .iData(iData),  
       .iEI(iEI),  
       .oData(oData),  
       .oEO(oEO)  
   );  
 
    
   initial begin  
     
       iData = 8'b00000000;  
       iEI = 1'b0;  
 
       // 等待一段时间以观察初始状态  
       #10;  
 
       // 测试序列开始  
       // 使能信号为0，输入全为0  
       #10 iData = 8'b00000000; iEI = 1'b0;  
       #10;  
 
       // 使能信号为1，输入d0为1  
       #10 iData = 8'b00000001; iEI = 1'b0;  
       #10;  
 
       // 输入d1为1  
       #10 iData = 8'b00000010; iEI = 1'b0;  
       #10;  
 
       // 输入d2为1  
       #10 iData = 8'b00000100; iEI = 1'b0;  
       #10;  
 
       #10 iData = 8'b00001100; iEI = 1'b0;  
       #10;
       
       #10 iData = 8'b00011100; iEI = 1'b0;  
       #10;
         
       #10 iData = 8'b11110111; iEI = 1'b0;  
       #10;  
 
   end  

endmodule

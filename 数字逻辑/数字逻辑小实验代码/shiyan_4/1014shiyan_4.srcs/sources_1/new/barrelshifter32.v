`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/14 16:34:40
// Design Name: 
// Module Name: barrelshifter32
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


module barrelshifter32(a,b,aluc,c);
    input [7:0] a;
    input [2:0] b;
    input [1:0] aluc;
    output reg [7:0] c;
    
     integer i; 
     reg [7:0] temp; 
     
    always@(*)begin
    case(aluc)
      2'b00:begin
                // 算术右移，先将输入数据转换为有符号数，然后进行右移
                c = $signed(a) >>> b;
            end
      2'b01:c=a<<b; //算术左移
      2'b10:c=a>>b;//逻辑右移
      2'b11:c=a<<b;//逻辑左移
      endcase
   end     
endmodule

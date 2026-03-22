`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/09/30 23:28:31
// Design Name: 
// Module Name: de_selector14
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


module de_selector14(
    input iC,
    input iS1,
    input iS0,
    output reg oZ0,
    output reg oZ1,
    output reg oZ2,
    output reg oZ3
    );
    always @(*) begin  
        // 默认情况，所有输出为1  
        oZ0 = 1;  
        oZ1 = 1;  
        oZ2 = 1;  
        oZ3 = 1;  
      
        // 根据选择信号分配输入数据到相应的输出  
        case ({iS1, iS0}) // 将iS1和iS0组合成一个2位的地址  
            2'b00: oZ0 = iC; // 当选择信号为00时，将iC分配到oZ0  
            2'b01: oZ1 = iC; // 当选择信号为01时，将iC分配到oZ1  
            2'b10: oZ2 = iC; // 当选择信号为10时，将iC分配到oZ2  
            2'b11: oZ3 = iC; // 当选择信号为11时，将iC分配到oZ3  
        endcase  
    end  
endmodule

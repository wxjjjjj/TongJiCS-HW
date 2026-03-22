`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/18 23:20:39
// Design Name: 
// Module Name: ram
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


module ram (
    input clk,
    input ena,
    input wena,
    input [4:0] addr,
    input [31:0] data_in,
    output reg [31:0] data_out
);
    reg [31:0] mem [31:0]; // 32个存储位置,每个位置32bit
     
        always @(posedge clk) begin
          if(ena)begin
             if(wena)begin//写
                mem[addr] <= data_in;
             end
             else begin//读
                data_out <= mem[addr];
             end
          end
          else begin
             data_out <=32'bz;
          end
        end
        
        always @(*) begin
          if(ena)begin
            if(wena==0)begin//读
              data_out <= mem[addr];
            end
          end
          else begin
            data_out <=32'bz;
          end
        end
endmodule
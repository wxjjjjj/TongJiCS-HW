`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/12 20:25:44
// Design Name: 
// Module Name: encoder83_Pri
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


module encoder83_Pri(iData,iEI,oData);
    input [7:0] iData;//d7-d0
    input iEI;
    output reg [2:0] oData;//y2-y0
    always @(*) begin  
        if (iEI == 1'b1) begin  
            oData = 3'b000;
        end else begin  
            // 使能输入为1，启用编码器  
            casez (iData)  
                8'b0???????: begin oData = 3'b000; end  
                8'b10??????: begin oData = 3'b001; end  
                8'b110?????: begin oData = 3'b010; end  
                8'b1110????: begin oData = 3'b011; end  
                8'b11110???: begin oData = 3'b100; end  
                8'b111110??: begin oData = 3'b101; end  
                8'b1111110?: begin oData = 3'b110; end  
                8'b11111110: begin oData = 3'b111; end
                8'b11111111: begin oData = 3'b000; end  
                default:     begin oData = 3'bxxx; end  
            endcase  
        end  
    end 
endmodule

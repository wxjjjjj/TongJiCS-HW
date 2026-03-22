`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/04 16:10:57
// Design Name: 
// Module Name: Counter8
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


module Counter8(
    input CLK,
    input rst_n,
    output  [2:0] oQ,
    output  [6:0] oDisplay
    );
    wire l0,l1,l2;
    wire out;
    
   Divider divv1(
        .I_CLK(CLK),
        .rst(rst_n),
        .O_CLK(out)
        );
        
    JK_FF f1(
          .CLK(out),
          .J(1),
          .K(1),
          .RST_n(rst_n),
          .Q1(l0)
          );
          
     JK_FF f2(
          .CLK(out),
          .J(l0),
          .K(l0),
          .RST_n(rst_n),
          .Q1(l1)
          );
          
      and (l2,l0,l1);
         
      JK_FF f3(
          .CLK(out),
          .J(l2),
          .K(l2),
          .RST_n(rst_n),
          .Q1(oQ[2])
          );
      
      assign oQ[0]=l0;
      assign oQ[1]=l1;    
      
      display7 display1(
            .iData(oQ),
            .oData(oDisplay)
            );
      
                 
endmodule

module JK_FF(
    input CLK,
    input J,
    input K,
    input RST_n,
    output reg  Q1,
    output reg  Q2
    );
    always@(posedge CLK or negedge RST_n)begin // ?
              if(!RST_n)begin //        ?   ?       ? ?0--        
               Q1<=0;
               Q2<=1;//Q2?    Q1 ?      
              end
              else begin
                 if(J==0&& K==0)begin
                  Q1<=Q1;
                  Q2<=Q2;
                 end
                 else if(J==0&&K==1)begin
                  Q1<=0;
                  Q2<=1;
                 end
                 else if(J==1&&K==0)begin
                  Q1<=1;
                  Q2<=0;
                 end
                 else begin
                  Q1<=Q2;
                  Q2<=Q1;
                 end
              end
    end

endmodule

module display7(iData,oData);
    input [2:0] iData;   //d2,d1,d0
    output reg [6:0] oData;   //g~a
    always @(*) begin  
           case (iData)   
                       3'b000: oData = 7'b1000000; 
                       3'b001: oData = 7'b1111001;  
                       3'b010: oData = 7'b0100100; 
                       3'b011: oData = 7'b0110000;  
                       3'b100: oData = 7'b0011001;  
                       3'b101: oData = 7'b0010010;  
                       3'b110: oData = 7'b0000010;  
                       3'b111: oData = 7'b1111000;  
                       default: oData = 7'b0000000; 
            endcase  
        end  
endmodule

module Divider(
    input I_CLK,
    input rst,
    output O_CLK
    );
    
    parameter N=10000000;
        reg [31:0]n=32'h00000000;
        reg q=0;
       always @(posedge I_CLK or negedge rst )begin
        if(rst==0)begin
          q<=0;
          n<=32'h00000000;
         end else begin
               if(n==(N/2-1)) begin
                        q<=~q;
                        n<=32'h00000000;
                end else
                        n<=n+1;
          end
       end
       assign O_CLK=q;
       
endmodule

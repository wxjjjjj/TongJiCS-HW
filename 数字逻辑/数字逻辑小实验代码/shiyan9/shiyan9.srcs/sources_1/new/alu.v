`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/11 15:45:35
// Design Name: 
// Module Name: alu
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


module alu(
    input [31:0] a,
    input [31:0] b,
    input [3:0] aluc,
    output reg [31:0] r,
    output reg zero,
    output reg carry,
    output reg negative,
    output reg overflow
    );
     always @(*) begin
           case (aluc)
               4'b0000: begin    //addu
                   r= a + b;
                   if(r==0)
                     zero=1;
                   else
                     zero=0;
                   if(r[31]==1)
                     negative=1;
                   else
                     negative=0; 
                   if(r<a||r<b)
                     carry=1;
                   else
                     carry=0;
               end
               4'b0001: begin //subu
                    r= a - b;
                    if(r==0)
                       zero=1;
                    else
                       zero=0;
                    if(r[31]==1)
                       negative=1;
                    else
                       negative=0; 
                    if(a<b)
                       carry=1;
                    else
                       carry=0; 
               end
               4'b0010: begin //add 有符号
                      r= $signed(a) + $signed(b);
                      if(r==0)
                         zero=1;
                      else
                         zero=0;
                         
                      if(r[31]==0)
                         negative=0;
                      else
                         negative=1;
                         
                      if(a[31]==0&&b[31]==0&&r[31]==1) //只有正数+正数/负数+负数才会溢出
                         overflow=1;
                      else if(a[31]==1&&b[31]==1&&r[31]==0)
                         overflow=1;
                      else
                         overflow=0;
               end
               4'b0011:begin //sub 有符号
                    r= $signed(a) - $signed(b);
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                      
                    if(r[31]==1)
                      negative=1;
                    else
                      negative=0;
                    
                    if(a[31]==0&&b[31]==1&&r[31]==1)
                      overflow=1;
                    else if(a[31]==1&&b[31]==0&&r[31]==0)
                      overflow=1;
                    else
                      overflow=0;
               end
               4'b0100:begin// and
                    r=a&b;
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                      
                    if(r[31]==1)
                      negative=1;
                    else
                      negative=0;
               end
               4'b0101:begin//or
                    r=a|b;
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                      
                   if(r[31]==1)
                      negative=1;
                   else
                      negative=0; 
               end
               4'b0110:begin //xor
                    r=a^b;
                    if(r==0)
                       zero=1;
                    else
                       zero=0;
                       
                    if(r[31]==1)
                       negative=1;
                     else
                       negative=0; 
               end
              4'b0111:begin //nor
                    r=~(a|b);
                    if(r==0)
                       zero=1;
                    else
                       zero=0;
                                           
                    if(r[31]==1)
                       negative=1;
                    else
                       negative=0;
              end
              4'b1000:begin //lui
                    r={b[15:0],16'b0};
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                      
                    if(r[31]==1)
                       negative=1;
                    else
                      negative=0;
              end
              4'b1001:begin//lui
                    r={b[15:0],16'b0};
                    if(r==0)
                       zero=1;
                    else
                       zero=0;
                                          
                    if(r[31]==1)
                       negative=1;
                    else
                       negative=0;
              end
              4'b1010:begin//sltu
                    r=(a<b)?1:0;
                    if(a-b==0)
                       zero=1;
                    else
                       zero=0;
                       
                    if(a<b)
                      carry=1;
                    else
                      carry=0;
                      
                    if(r[31]==1)
                       negative=1;
                    else
                       negative=0;
              end
              4'b1011:begin //slt 有符号数比较
                    r=($signed(a) < $signed(b))?1:0;
                    if($signed(a) - $signed(b)==0)
                       zero=1;
                    else
                       zero=0;
                       
                    if($signed(a) - $signed(b)<0)
                      negative=1;
                    else
                     negative=0;
                       
              end
              4'b1100:begin//sra--算术右移
                    r=$signed(b)>>>a;  //算术右移，一定是先把要移动的数转成有符号数再移！！
                            
                    if(a<=32&&a>=1)
                       carry=b[a-1];
                    else if(a>32)
                       carry=b[31]; //符号位
                    else
                       carry=0;
                       
                       
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                                                              
                   if(r[31]==1)
                      negative=1;
                   else
                      negative=0;
              end
              4'b1101:begin//srl--逻辑右移
                    r=b>>a;
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                      
                    if(r[31]==1)
                       negative=1;
                    else
                       negative=0;
                                            
                    if(a<=32&&a>=1)
                       carry=b[a-1];
                     else 
                       carry=0;
              end
              4'b1110:begin//sll/sla--逻辑/算术左移
                    r=b<<a;
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                                          
                    if(r[31]==1)
                       negative=1;
                    else
                       negative=0;
                                                                
                    if(a<=32&&a>=1)
                       carry=b[32-a];
                    else 
                       carry=0;
              end  
              4'b1111:begin//sll/sla--逻辑/算术左移
                    r=b<<a;
                    if(r==0)
                      zero=1;
                    else
                      zero=0;
                                                              
                    if(r[31]==1)
                       negative=1;
                    else
                       negative=0;
                                                                                    
                    if(a<=32&&a>=1)
                       carry=b[32-a];
                   else 
                       carry=0;
              end
               default: begin
                   r = 32'b0; 
               end
           endcase
       end
    
     
endmodule

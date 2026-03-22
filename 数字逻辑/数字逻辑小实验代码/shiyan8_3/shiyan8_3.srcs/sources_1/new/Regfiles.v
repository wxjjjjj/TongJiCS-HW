`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/11/19 00:38:56
// Design Name: 
// Module Name: Regfiles
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


module Regfiles(
    input clk,
    input rst,
    input we,
    input [4:0] raddr1,
    input [4:0] raddr2,
    input [4:0] waddr,
    input [31:0] wdata,
    output [31:0] rdata1,
    output [31:0] rdata2
    );
    wire [31:0]out[31:0];
    wire [31:0]wa;
    decoder dwa(.iData(waddr),.iEna(we),.oData(wa));
    genvar i;
    generate
    for(i=0;i<32;i=i+1)
    begin:bin
        reg32 inst(.clk(clk),.rst(rst),.ena(wa[i]),.data_in(wdata),.data_out(out[i]));
    end
    endgenerate
    selector32 s1(
        .iC00(out[0]),.iC01(out[1]),.iC02(out[2]),.iC03(out[3]),.iC04(out[4]),.iC05(out[5]),.iC06(out[6]),.iC07(out[7]),
        .iC08(out[8]),.iC09(out[9]),.iC10(out[10]),.iC11(out[11]),.iC12(out[12]),.iC13(out[13]),.iC14(out[14]),.iC15(out[15]),
        .iC16(out[16]),.iC17(out[17]),.iC18(out[18]),.iC19(out[19]),.iC20(out[20]),.iC21(out[21]),.iC22(out[22]),.iC23(out[23]),
        .iC24(out[24]),.iC25(out[25]),.iC26(out[26]),.iC27(out[27]),.iC28(out[28]),.iC29(out[29]),.iC30(out[30]),.iC31(out[31]),
        .iS(raddr1),.ena(~we),.oZ(rdata1)
        );
    selector32 s2(
        .iC00(out[0]),.iC01(out[1]),.iC02(out[2]),.iC03(out[3]),.iC04(out[4]),.iC05(out[5]),.iC06(out[6]),.iC07(out[7]),
        .iC08(out[8]),.iC09(out[9]),.iC10(out[10]),.iC11(out[11]),.iC12(out[12]),.iC13(out[13]),.iC14(out[14]),.iC15(out[15]),
        .iC16(out[16]),.iC17(out[17]),.iC18(out[18]),.iC19(out[19]),.iC20(out[20]),.iC21(out[21]),.iC22(out[22]),.iC23(out[23]),
        .iC24(out[24]),.iC25(out[25]),.iC26(out[26]),.iC27(out[27]),.iC28(out[28]),.iC29(out[29]),.iC30(out[30]),.iC31(out[31]),
        .iS(raddr2),.ena(~we),.oZ(rdata2)
        );
endmodule

module Dff(
    input CLK,
    input D, //输入信号D
    input RST_n, //复位信号，低电平有效
    input ena,
    output reg Q1 //输出信号Q
    );
    always @(negedge CLK or posedge RST_n)
    begin
        if(RST_n==1)
        begin
            Q1<=0;
        end
        else
        begin
            if(ena==1)
            begin
                Q1<=D;
            end
        end
    end
endmodule

module reg32(
    input clk, //1 位输入，寄存器时钟信号，上升沿时为PC 寄存器赋值
    input rst, //1 位输入，异步重置信号，高电平时将PC 寄存器清零
               //注：当ena 信号无效时，rst 也可以重置寄存器
    input ena, //1 位输入,有效信号高电平时PC 寄存器读入data_in
               //的值，否则保持原有输出
    input [31:0] data_in, //32 位输入，输入数据将被存入寄存器内部
    output [31:0] data_out //32 位输出，工作时始终输出 PC
                               //寄存器内部存储的值
    );
    genvar i;
    generate
    for(i=0;i<32;i=i+1)
    begin:bin
        Dff inst(.CLK(clk),.RST_n(rst),.D(data_in[i]),.ena(ena),.Q1(data_out[i]));
    end
    endgenerate
endmodule

module selector32(
    input [31:0] iC00,
    input [31:0] iC01,
    input [31:0] iC02,
    input [31:0] iC03,
    input [31:0] iC04,
    input [31:0] iC05,
    input [31:0] iC06,
    input [31:0] iC07,
    input [31:0] iC08,
    input [31:0] iC09,
    input [31:0] iC10,
    input [31:0] iC11,
    input [31:0] iC12,
    input [31:0] iC13,
    input [31:0] iC14,
    input [31:0] iC15,
    input [31:0] iC16,
    input [31:0] iC17,
    input [31:0] iC18,
    input [31:0] iC19,
    input [31:0] iC20,
    input [31:0] iC21,
    input [31:0] iC22,
    input [31:0] iC23,
    input [31:0] iC24,
    input [31:0] iC25,
    input [31:0] iC26,
    input [31:0] iC27,
    input [31:0] iC28,
    input [31:0] iC29,
    input [31:0] iC30,
    input [31:0] iC31,
    input [4:0] iS,
    input ena,//高电平有效，可以读数据
    output [31:0] oZ
    );
    reg [31:0]temp[31:0];
    reg [31:0]t;
    always @(*)
    begin
        temp[0]=iC00;
        temp[1]=iC01;
        temp[2]=iC02;
        temp[3]=iC03;
        temp[4]=iC04;
        temp[5]=iC05;
        temp[6]=iC06;
        temp[7]=iC07;
        temp[8]=iC08;
        temp[9]=iC09;
        temp[10]=iC10;
        temp[11]=iC11;
        temp[12]=iC12;
        temp[13]=iC13;
        temp[14]=iC14;
        temp[15]=iC15;
        temp[16]=iC16;
        temp[17]=iC17;
        temp[18]=iC18;
        temp[19]=iC19;
        temp[20]=iC20;
        temp[21]=iC21;
        temp[22]=iC22;
        temp[23]=iC23;
        temp[24]=iC24;
        temp[25]=iC25;
        temp[26]=iC26;
        temp[27]=iC27;
        temp[28]=iC28;
        temp[29]=iC29;
        temp[30]=iC30;
        temp[31]=iC31;
        if(ena==1)
            t<=temp[iS];
        else
            t=32'bz;
    end
    assign oZ=t;
endmodule

module decoder(
    input [4:0] iData,
    input iEna,
    output [31:0] oData
    );
    genvar i;
    generate 
    for(i=0;i<32;i=i+1)
    begin:bit
        assign oData[i]=(iData==i)&&(iEna==1)?1'b1:1'b0;
    end
    endgenerate
endmodule

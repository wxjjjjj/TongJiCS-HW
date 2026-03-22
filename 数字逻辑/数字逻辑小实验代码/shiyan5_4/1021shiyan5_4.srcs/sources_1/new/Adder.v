`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2024/10/21 20:08:16
// Design Name: 
// Module Name: Adder
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


module Adder(
    input [7:0] iData_a,
    input [7:0] iData_b,
    input iC,
    output [7:0] oData,
    output oData_C
    );
    wire l1,l2,l3,l4,l5,l6,l7;
    
    FA f1(  
              .iA(iData_a[0]),  
              .iB(iData_b[0]),
              .iC(iC),
              .oS(oData[0]),
              .oC(l1) 
             );  
     FA f2(  
               .iA(iData_a[1]),  
               .iB(iData_b[1]),
               .iC(l1),
               .oS(oData[1]),
               .oC(l2) 
              );  
     FA f3(  
               .iA(iData_a[2]),  
               .iB(iData_b[2]),
               .iC(l2),
               .oS(oData[2]),
               .oC(l3) 
               ); 
     FA f4(  
               .iA(iData_a[3]),  
               .iB(iData_b[3]),
               .iC(l3),
               .oS(oData[3]),
               .oC(l4) 
                ); 
     FA f5(
                .iA(iData_a[4]), 
                .iB(iData_b[4]),
                .iC(l4),
                .oS(oData[4]),
                .oC(l5) 
                 ); 
      FA f6(  
                .iA(iData_a[5]),  
                .iB(iData_b[5]),
                .iC(l5),
                .oS(oData[5]),
                .oC(l6) 
                 );
     FA f7(  
                 .iA(iData_a[6]),  
                 .iB(iData_b[6]),
                 .iC(l6),
                 .oS(oData[6]),
                 .oC(l7) 
                  ); 
     FA f8(  
                 .iA(iData_a[7]),  
                 .iB(iData_b[7]),
                 .iC(l7),
                 .oS(oData[7]),
                 .oC(oData_C) 
                  );  
endmodule

module FA(
    input iA,
    input iB,
    input iC,
    output  oS,
    output  oC
    );
    wire xor1,and1,and2;
     
    xor (xor1,iA,iB);
    xor (oS,xor1,iC);
    and(and1,iC,xor1);
    and(and2,iA,iB);
    or(oC,and1,and2); 
 
    
endmodule


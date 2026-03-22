DATA     SEGMENT
_P            Dw       11
_Q           Dw       23
N            DW	      ?
E		DW		3
T		DW		?
D		DW		?

KK  		DW		 ?
LL  		DW 		? 
aa 		DW		 ?
 bb 		DW		 ?
 q		 DW 		 ?
RR 		DW 		 ?
k1 		DW 		 1
l1 		DW		 0
 k2		 DW 		0
l2 		 DW		 -1

BUFFER1	 DB	 10,0,10 DUP(0)
BUFFER2	DB	10,0,10 DUP(0)
LINE     DB  13,10,"$"
msg1 DB 13,10,'The input string: ',13,10,'$'
msg2	db	13,10,'THE Encrypted STRING: ',13,10,'$'
msg3	db	13,10,'the secret key is:',13,10,'$'
msg4	db	13,10,'the public key is: ',13,10,'$'
msg5	db	'e= ','$'
msg6	db	13,10,'n= ','$'
msg7	db	'd=','$'
msg8	db	13,10,'the decrypted string is:',13,10,'$'
DATA   ENDS

CODE  SEGMENT
      ASSUME  CS:CODE,DS:DATA
START:  MOV     AX,DATA
	MOV     DS,AX

	MOV	Ax,_P
	MUL		_Q			;乘积存在AX内

	MOV	N,AX		;N目前值为FD(253)

	sub		_p,1
	sub		_q,1
	mov		ax,_p
	mul		_q			;乘积存在AX内	

	MOV	T,AX		; φ就是T=(P-1)*(Q-1)(DC)/220

	MOV DX,OFFSET MSG4
       MOV AH,09H
        INT 21H				;显示提示信息

	MOV DX,OFFSET MSG5
       MOV AH,09H
        INT 21H				;显示提示信息e

		MOV AX,e
                MOV  BX,10           ;十进制数
                XOR CX,CX
OLOC:
               MOV 	DX,0
               DIV    BX
                PUSH DX
               INC   CX
               CMP AX,0
               JNZ  OLOC
OLOC2:
               POP DX
               ADD DL,'0'
              MOV   AH,2
               INT   21H
              LOOP OLOC2

	MOV DX,OFFSET MSG6
       MOV AH,09H
        INT 21H				;显示提示信息n

		MOV AX,n
                MOV  BX,10           ;十进制数
                XOR CX,CX
nLOC:
               MOV 	DX,0
               DIV    BX
                PUSH DX
               INC   CX
               CMP AX,0
               JNZ  nLOC
nLOC2:
               POP DX
               ADD DL,'0'
              MOV   AH,2
               INT   21H
              LOOP nLOC2

	
	mov ax, e	;e取值是3   
    mov aa, ax  		;AA [0004]
    mov bx, t 	;φ(T)是220
    mov bb, bx			;BB [0006]

loc:
	XOR	 DX,DX
	mov		ax,aa
	idiv	bb	;商在ax里，余数在dx里
	cmp		dx,0
	jz		mm			;如果余数是0，跳转输出结果
	MOV	Q,AX		;Q [0008]
	MOV	RR,DX		;RR [000A]
	IMUL	K2	;AX乘k2，存在dx：ax里 K2 [0010]
	MOV	BX,K1		;K1 [000C]
	SUB		BX,AX		;bx=1
	MOV	KK,BX
	MOV	AX,Q	                                                    
	IMUL	L2			;L2 [0012]
	MOV	BX,L1
	SUB		BX,AX		;bx=0
	MOV	LL,Bx

	mov		bx,bb
	mov		aa,bx

	mov		bx,rr
	mov		bb,bx

	mov		bx,k2
	mov		k1,bx

	mov		bx,l2
	mov		l1,bx

	mov	 	bx,kk
	mov		k2,bx
	
	CMP	K2,0
	JL		LOC1		;k2小于0，则跳转

	mov		bx,ll
	mov		l2,bx
	JMP		LOC


 mm:
	
MOV DX,OFFSET MSG3
       MOV AH,09H
        INT 21H				;显示提示信息


	MOV DX,OFFSET MSG7
       MOV AH,09H
        INT 21H				;显示提示信息d

	MOV AX,k2
                MOV  BX,10           ;十进制数
                XOR CX,CX
qLOC:
               MOV 	DX,0
               DIV    BX
                PUSH DX
               INC   CX
               CMP AX,0
               JNZ  qLOC
qLOC2:
               POP DX
               ADD DL,'0'
              MOV   AH,2
               INT   21H
              LOOP qLOC2

	MOV DX,OFFSET MSG6
       MOV AH,09H
        INT 21H				;显示提示信息n

	MOV AX,n
                MOV  BX,10           ;十进制数
                XOR CX,CX
mLOC:
               MOV 	DX,0
               DIV    BX
                PUSH DX
               INC   CX
               CMP AX,0
               JNZ  mLOC
mLOC2:
               POP DX
               ADD DL,'0'
              MOV   AH,2
               INT   21H
              LOOP mLOC2

	JMP	_GO_	

 LOC1:
	MOV	BX,K2
	ADD	BX,220
	mov		k2,bx

	MOV	BX,L2
	ADD	BX,3
	mov		l2,bx

	JMP		LOC	

_GO_:
	MOV DX,OFFSET MSG1
       MOV AH,09H
        INT 21H				;显示提示信息

	MOV  	AH,0AH
	LEA		DX,BUFFER1	;buffer1的地址是000A
	INT	21H				;接收用户输入到缓冲区


	MOV	SI,OFFSET BUFFER1
	MOV	DI,OFFSET	BUFFER2
	MOV	CX,10			;挨个处理输入的字符		

NEXT:
	XOR	        AX,AX
	MOV	AL,[SI+2]
	mov		bx,ax
LOC2:
	MUL		BX
	DEC		E
	CMP	E,1
	JNZ		LOC2
	DIV		N			;余数在dx，商在ax
	xor		AX,AX
	MOV	AX,DX
	MOV	[DI+2],AL
	INC	SI
	INC	DI
	MOV	E,3
	LOOP	NEXT

; 输出提示信息
  mov ah, 09h
 lea dx, msg2
 int 21h

;输出缓冲区的字符
	MOV	SI,OFFSET	BUFFER2+2
	MOV	BX,0
NEXT_CHAR:
	MOV	AL,[SI]
	CMP	AL,0		;判断是否到达字符串结束
	JE	END_OUTPUT
	MOV	AH,02H		;单个字符输出功能
	MOV	DL,AL
	INT	21H
	INC	SI
	INC	BX
	JMP	NEXT_CHAR
END_OUTPUT:

	; 输出提示信息
  	mov ah, 09h
	 lea dx, msg8			;再解密
	int 21h

	; 输出缓冲区中的字符串
    mov si, offset buffer1 + 2  ; 指向实际输入字符的起始位置
    mov bx, 0  ; 用于计数实际输入的字符数
nnext_char:
    mov al, [si]
    cmp al, 0  ; 判断是否到达字符串结束
    je eend_output
    mov ah, 02h  ; 单个字符输出功能
    mov dl, al
    int 21h
    inc si
    inc bx
    jmp nnext_char
eend_output:


     	 MOV        AX,4C00H
        INT            21H
CODE     ENDS
            END       START
MOV R1,P0 ; Input
MOV DPTR,#numbers

MOV A,R1 ; For tetr move

MOV R0, #4 ; Cycle iterations
ANL A, #11110000b ; Clear last 4 tetrades
MoveTetrRight:
RR A
DJNZ R0,MoveTetrRight

MOV R0,A ; Save 4 first tetr
MOV A,R1 ; Get input
ANL A, #00001111b ; Get 4 last tetr
MOV R1,A
ADD A,R0 ; Get summ of tetrades

SUBB A, #110b
JC calculate
JZ calculate

MOV P1, #11001000b ; Symbol «П»
PF:
jmp $ ; Cycling in the end

calculate:
MOV A,R0
MOV B,R1
MUL AB
movc A, @A+dptr
MOV P1,A
JMP PF



numbers: ; Used for output
db 11000000b ; 0
db 11111001b ; 1
db 10100100b ; 2
db 10110000b ; 3
db 10011001b ; 4
db 10010010b ; 5
db 10000010b ; 6
db 11111000b ; 7
db 10000000b ; 8
db 10010000b ; 9
end

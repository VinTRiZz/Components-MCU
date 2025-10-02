ORG 0
MC:
MOV P0, #00001111b
MOV A, P0
ANL A,#00001111b
MOV R0,A

MOV P0,#11110000b
MOV A, P0
ANL A, #11110000b
ADD A, R0
ANL A, #01000100b
JNZ MC



COLUMN EQU 01 ;Начальный номер стобца матрицы
ROW EQU 0 ;начальный номер ряда матрицы
TABL EQU 50h ; адрес таблицы скан-кода символа
mov a, #COLUMN
mov b, #ROW
mov dptr, #TABL
mov r0, #0 ;установка счетчика столбцов

m1: call led ; вызов подпрограммы определения скан-кода
mov p2, a ;подключения одного столбца
mov p1, b ;и всех рядов матрицы
call loop ;задержка на 2,5 мс (частота миганий 50 ГЦ)
rl a ;следующий столбец
inc r0 ;икримент счетчика столбцов
orl p1, #0ffh ;гашение текущего столбца
cjne r0, #8, m1 ;продолжать до конца столбцов матрицы
mov r0, #0 ;начинается
mov r1, #0 ; следующий
jmp MC ; цикл

led: push acc
mov a, r1
movc a, @A+DPTR
mov b, a
inc r1
pop acc
ret
loop: MOV R2, #01h
MOV R3, #01h
DJNZ R3, $
DJNZ R2, $-5
ret
org 50h
db 11111111b
db 10110001b
db 01101110b
db 01101110b
db 01101110b
db 10000001b
db 11111111b
db 11111111b
end

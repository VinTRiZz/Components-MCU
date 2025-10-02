org 0
jmp start
RS bit P2.2 ;бит команда данные
RW bit P2.1 ;бит чтения/записи
E bit P2.0 ;бит строба
D equ P1 ;порт данные
;Макрос записи данных/команд в HD44780
hd macro hd_code
setb E
mov D, hd_code
clr E
endm ;конец макроса
string:
db 'Lazarev'
db 'Aleksey'
start:
mov D, #0
clr RW
;Инициализация дисплея
mov DPTR, #string
main: clr RS
hd #00000001b ; Очистить дисплей
hd #00000010b ; Установить курсор в начало
hd #00000110b ; Выбор направления курсора
hd #00001100b ; Включение дисплея и отключение кусора (2 ласт бита)
hd #00011110b ; Сдиг курсора вправо
hd #00111100b ; Определение параметров развертки
hd #10000001b ; Определение начального адреса в
;DDRAM
;Конец инициализации

setb RS
mov R0, #0 ; Печать "Лазарев"
print1:
mov A, R0
INC R0
movc A, @A+DPTR
hd A
cjne A, #'v', print1

; Установка указателя на следующую строку
clr RS
hd #11000001b
setb RS

; Печать "Алексей"
mov R0, #7
print2:
mov A, R0
INC R0
movc A, @A+DPTR
hd A
cjne A, #'y', print2

; Переход на новый круг Ада
jmp main

end

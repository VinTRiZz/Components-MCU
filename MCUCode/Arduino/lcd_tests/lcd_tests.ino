// #define MAIN_PROGRAM
 #define TEST

#define BLANK_PRG // For safe change connections

#ifdef MAIN_PROGRAM
#ifdef BLANK_PRG
#undef BLANK_PRG
#endif // BLANK_PRG


#include <GyverOLED.h>

// OLED Library object
GyverOLED<SSH1106_128x64> oled;

void lcd_print(int com_number);

int com_number = 0;

void setup() {
  for (int i = 3; i < 12; i++)
    {
      pinMode(i,OUTPUT);
    }
    
    oled.init();
    oled.clear();
    oled.setCursor(0,1);
    oled.print("Work");
    oled.update();
}

void loop() {
  oled.setCursor(0,0);
  oled.print("Current command: ");
  oled.print(com_number);
  oled.update();
  delay(1000);

  lcd_print(com_number);
}

void lcd_print(int com_number)
{
  
}

#endif // MAIN_PROGRAM
// -*--------------------------------------------------**************







#ifdef TEST
#ifdef BLANK_PRG
#undef BLANK_PRG
#endif // BLANK_PRG

// Подключаем стандартную библиотеку LiquidCrystal
#include <LiquidCrystal.h>
 
// Инициализируем объект-экран, передаём использованные 
// для подключения контакты на Arduino в порядке:
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(4, 5, 9, 10, 11, 12);
 
void setup() 
{
    // устанавливаем размер (количество столбцов и строк) экрана
    lcd.begin(16, 2);
    // печатаем первую строку
    lcd.print("Hello world!");
    // устанавливаем курсор в колонку 0, строку 1. То есть на
    // самом деле это вторая строка, т.к. нумерация начинается с нуля
    lcd.setCursor(0, 1);
    // печатаем вторую строку
    lcd.print("Hello Kitty");
}
 
void loop() 
{
}


#endif // TEST







#ifdef BLANK_PRG
void setup()
{
  
}
void loop()
{
  
}
#endif // BLANK_PRG

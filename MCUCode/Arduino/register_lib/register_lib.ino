    // Библиотека для работы с текстовым дисплеем
    #include <LiquidCrystal.h>
     
    // Задаём имя пинов дисплея
    constexpr uint8_t PIN_RS = 7;
    constexpr uint8_t PIN_EN = 6;
    constexpr uint8_t PIN_DB4 = 5;
    constexpr uint8_t PIN_DB5 = 4;
    constexpr uint8_t PIN_DB6 = 3;
    constexpr uint8_t PIN_DB7 = 2;
     
    /*
     * Создаём объект для работы с текстовым дисплеем
     * LiquidCrystal lcd(RS, EN, DB4, DB5, DB6, DB7);
     * Подробности подключения:
     * http://wiki.amperka.ru/products:display-lcd-text-16x2
    */
     
    LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
     
    void setup() {
      // Устанавливаем размер экрана
      // Количество столбцов и строк
      lcd.begin(16, 2);
      // Устанавливаем курсор в колонку 0 и строку 0
      lcd.setCursor(0, 0);
      // Печатаем первую строку
      lcd.print("Hello, world!");
      // Устанавливаем курсор в колонку 0 и строку 1
      lcd.setCursor(0, 1);
      // Печатаем вторую строку
      lcd.print("Do It Yourself");
    }
     
    void loop() {
    }



#ifdef TST

// Shift register pins
int8_t  ST_CP = 3,  // latch
        SH_CP = 4,  // Clock
        DS = 2;     // Data

void shiftRegisterSet(int8_t what);

void setup() {
  // Preset for shift register
  pinMode(ST_CP, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(DS, OUTPUT);
  shiftRegisterSet(0);
}
void loop()
{
  
  
  delay(100000);
}

void shiftRegisterSet(int8_t what) // Work with SN74HC595N
{
  // Open
  digitalWrite(ST_CP, LOW);
  // Send
  shiftOut(DS, SH_CP, MSBFIRST, what);
  // Close with show
  digitalWrite(ST_CP, HIGH);
}

#endif // TST

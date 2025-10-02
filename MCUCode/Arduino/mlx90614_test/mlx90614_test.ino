// #define I2C_SCANNER
// #define MAIN_PROGRAM
// #define TEST

#define BLANK_PRG // For safe change connections

#ifdef MAIN_PROGRAM /// ----------------------------------------------------------------------------------- /// 
#ifdef BLANK_PRG
#undef BLANK_PRG
#endif // BLANK_PRG

#define USB_SEND // Switch for sending by serial

#define MEASURE_DELAY_PERIOD 10
#define BUTTON_PIN 2        // Todo: Check datasheet and change
#define LED_PIN 3           // Todo: Check datasheet and change
#define CHECK_INTERVAL 30   // Checks connections every this loop

#include <Adafruit_MLX90614.h>
#include <GyverOLED.h>

// OLED Library object
GyverOLED<SSH1106_128x64> oled;
// GyverOLED<SSD1306_128x32, OLED_BUFFER> oled;

// For check modules are valid
int display_valid = 0;

#ifdef USB_SEND
  int pc_connect_valid = 0;
  #define SERIAL_TIMEOUT 2000 // in ms
#endif // USB_SEND

int mlx90614_valid = 0;

int loop_number = 0; // Every CHECK_INTERVAL loop checks devices

// Declaration for an MLX90614 sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void checkDevices();

void getTemperature();

void setup() 
{
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);

#ifdef USB_SEND
  Serial.begin(9600);
#endif // USB_SEND

  checkDevices();

#ifdef USB_SEND
  Serial.println("Результат инициализации:");
  if (display_valid)
    Serial.println("Дисплей найден.");
    else
    Serial.println("Дисплей не найден.");
    
  if (mlx90614_valid)
    Serial.println("Датчик найден.");
    else
    Serial.println("Датчик найден.");
#endif // USB_SEND

  delay(2000); // Final delay
  oled.clear();
}

void loop() 
{
  getTemperature();

  // Check if all still connected
  if (loop_number > CHECK_INTERVAL)
  {
    checkDevices();
    loop_number = 0;
  }
  else
    loop_number++;
    
  delay(MEASURE_DELAY_PERIOD);
}
void checkDevices()
{
  // Show trying to connect devices
  digitalWrite(LED_PIN, HIGH);
  delay(1);
  
  if (!mlx.begin()) 
  {
    mlx90614_valid = 0;
  }
  else
  {
    // Crutch
    oled.setCursor(0, 0);
    oled.print("                    ");
    mlx90614_valid = 1;
  }

  // Check display
  Wire.beginTransmission(0x3C);
  int temp = Wire.endTransmission();
  Wire.beginTransmission(0x3C+1);

  if (temp == 0 && Wire.endTransmission() != 0 )
    {
      if (!display_valid)
      {
        oled.init();
        oled.clear();
        oled.setScale(1);
        oled.update();
        display_valid = 1;
      }
    }
    else
    {
      display_valid = 0;

      for (int i = 0; i < 3; i++) // Blink
      {
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
      }
    }

#ifdef USB_SEND
  if (!Serial.available())
  {
    Serial.begin(9600);
    for (int i = 0; i < SERIAL_TIMEOUT; i++)
    {
      delay(1);
      pc_connect_valid = Serial;
      if (pc_connect_valid)
      {
        oled.setCursor(0, 2);
        oled.print("               ");
        oled.setCursor(0, 3);
        oled.print("               ");
        delay(1);
        break;
      }
    }
  }
    Serial.println("Ошибка: Датчик не найден.");
  if (pc_connect_valid && !display_valid)
    Serial.println("Ошибка: Дисплей не найден.");

  if (!pc_connect_valid)
  {
    oled.setCursor(0, 2);
    oled.print("Потеряна связь");
    oled.setCursor(0, 3);
    oled.print("с компьютером.");
    oled.update();
  }
#endif // USB_SEND

  // End if trying to connect devices
  digitalWrite(LED_PIN, LOW);
}

void getTemperature()
{
  float object_temp = 0;
  
  if (mlx90614_valid)
  {
    // ambient_temp = mlx.readAmbientTempC();
    object_temp  = mlx.readObjectTempC();

    if (display_valid)
    {
      // Remember temperature
    if (!digitalRead(BUTTON_PIN))
    {
      digitalWrite(LED_PIN, HIGH);
      oled.setScale(1);
      oled.setCursor(0, 1);
  
      oled.print("Сохранено:  ");
      oled.print(object_temp);
      
      oled.update();
  #ifdef USB_SEND
      if (pc_connect_valid)
      {
        Serial.print("Температура: "); Serial.print(object_temp); Serial.println(" C");
        Serial.println();
      } 
  #endif // USB_SEND
      // Respond about remember
      delay(10);
      digitalWrite(LED_PIN, LOW);
    }
      
      oled.setScale(1);
      oled.setCursor(0, 0);
      
      oled.print("Текущая:    ");
      oled.print(object_temp);
      
      oled.update();
    }
  }
  else
  {
    if (display_valid)
    {
      oled.clear();
      oled.setScale(1);
      oled.setCursor(0, 0);
      
      oled.println("Датчик не найден.");
      
      oled.update();
    }
  }
}
#endif // MAIN_PROGRAM /// ----------------------------------------------------------------------------------- /// 





// -------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------- //

#ifdef BLANK_PRG
void setup()
{
  
}
void loop()
{
  
}
#endif // BLANK_PRG


#ifdef TEST /// ----------------------------------------------------------------------------------- /// 
#ifdef BLANK_PRG
#undef BLANK_PRG
#endif // BLANK_PRG



#endif // TEST /// ----------------------------------------------------------------------------------- /// 

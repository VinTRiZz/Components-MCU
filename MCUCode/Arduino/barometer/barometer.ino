#include <GyverOLED.h>

#include <SFE_BMP180.h>

GyverOLED<SSD1306_128x64> oled;

SFE_BMP180 barometer;

#define AVERAGE_PRESSURE 750
#define SUNNY_PRESSURE AVERAGE_PRESSURE + 5
#define RAINY_PRESSURE AVERAGE_PRESSURE - 5

#define HISTORY_SIZE 64

#define MIN_TEMP 20.0

#define MIN_PRESS 740.0

double pressure = 0;
double temperature = 0;

uint32_t pressure_history[HISTORY_SIZE] = {0};
int16_t temperature_history[HISTORY_SIZE] = {0};

void setup() 
{
  oled.init();
  barometer.begin();
}

void loop() 
{
  if (getPressure() == 0)
  {
    oled.setCursor(0,1);
    oled.print("P: ");
    oled.print(pressure);
    oled.print("   T: ");
    oled.print(temperature);

    pressure_history[HISTORY_SIZE - 1] = pressure * 10;

    temperature_history[HISTORY_SIZE - 1] = temperature * 100;

    for (int8_t i = 0; i < HISTORY_SIZE - 1; i++) 
    {
        pressure_history[i] =  pressure_history[i + 1];
        temperature_history[i] = temperature_history[i + 1];
    }    
    
    oled.setCursor(0,0);
    if (pressure <= RAINY_PRESSURE)
    {
      oled.print("Rain garanted");
    }
    else if (pressure >= SUNNY_PRESSURE)
    {
      oled.print("HEAD! ");
      oled.print("Sun garanted");
    } else
    {
      oled.print("Normal");
    }

    printHistory();
  }
  else
  {
    oled.setCursor(0,1);
    oled.print("Error");
  }
  oled.update();

  delay(30000);
  oled.clear();
}




int getPressure()
{
    char status;

    status = barometer.startTemperature();
    if (status != 0)
    {
        // ожидание замера температуры
        delay(status);
        status = barometer.getTemperature(temperature);
        if (status != 0)
        {
            status = barometer.startPressure(3);
            if (status != 0)
            {
                // ожидание замера давления
                delay(status);
                status = barometer.getPressure(pressure, temperature);
                pressure /= 1.333; /// 1.333 гПа == 1 мм рт ст
                if (status != 0)
                {
                    return 0;
                }
            }
        }
    }
    return -1;
}

void printHistory()
{
  for (int8_t i = 0; i < 127; i++)
  {
    oled.dot(i, 63);
    oled.dot(i, 20);
  }

  for (int8_t i = 0; i < 52; i++)
  {
    oled.dot(64, i + 20);
  }

  int8_t sync = 0; // For small graphics
  for (int8_t i = 0; i < HISTORY_SIZE; i++)
  {
    for (int8_t j = 0; j < 1; j++)
      oled.dot(i + sync + j, 64 - (pressure_history[i] / 10.0 - MIN_PRESS) * 1.5);

    for (int8_t j = 0; j < 1; j++)
      oled.dot(i + sync + j + 64, 64 - (temperature_history[i] / 100.0 - MIN_TEMP) * 2.0);

      sync += 0;
  }
}

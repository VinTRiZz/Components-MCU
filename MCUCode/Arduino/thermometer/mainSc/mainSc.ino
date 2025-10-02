// #define DISPLAY_TEST // -----------------------------------------------------------------------------------------------
// #define I2C_SCANNER
// #define MAIN
 #define TEST

#ifdef TEST

#include "ssd1306.h"

void setup() {

Serial.begin(9600);

SSD1306_Init();
/*
  for (int i = 0; i<10 ; i++)
  {
    Serial.println("SSD1306 get attempt...");
    if ( !SSD1306_Init() )
      Serial.println("SSD1306 err");
      else
      {
       Serial.println("SSD1306 connect success");
       break;
      }
    delay(100);
  }
  */
  SSD1306_Clear();/*
  Serial.println("SSD1306 Cleared");*/
  delay(5);
  
}

void loop() 
{
  
  // SSD1306_GotoXY(0,0);
  // SSD1306_Puts("TEST INFO:\n", &Font_7x10,1);
  // SSD1306_Puts("ТЕСТ ИНФО:\n",&Font_7x10,1);
  /*
  for (int i = 0; i < 100; i++)
    SSD1306_DrawPixel(i, i, SSD1306_COLOR_WHITE);*/

  SSD1306_Fill(SSD1306_COLOR_BLACK);
  delay(1);
  SSD1306_UpdateScreen();
  Serial.println("SSD1306 Filled black");
  delay(1000);

  SSD1306_Fill(SSD1306_COLOR_WHITE);
  delay(1);
  SSD1306_UpdateScreen();
  Serial.println("SSD1306 Filled white");
  delay(1000);
  
}

#endif // TEST






#ifdef DISPLAY_TEST // -----------------------------------------------------------------------------------------------

/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 256 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1,/* Freq */ 400000UL, /* IDK what*/ 100000UL);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.fillScreen(BLACK);
  // Display static text
  display.println("Hello, world!");
  display.display(); 
}

void loop() {
  
}



#endif // DISPLAY_TEST // -----------------------------------------------------------------------------------------------

#ifdef MAIN
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
/*
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
*/

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define MLX_90614_ADDRESS 0x5A // Found by scanner

int count = 0;        // A counter variable to navigate through the input[] character array
boolean flag = 0;     // A variable to store the Tag match status

long duration;
int distance;
float TempReading;

void setup()
{
  
  Serial.begin(9600);
  while (!Serial); // Waiting for Serial Monitor
  Serial.print("Started\n");
/*
  Wire.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextSize(1);

  display.clearDisplay();
  while(1)
  {
    Serial.println("Show it");
    display.println("T");
    display.display();
    delay(1000);
    display.clearDisplay();
    Serial.println("Cleared");
    delay(1000);
  }
  */
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  mlx.begin();
  
}
void loop()
{
  /*
  testDrawLine();

  Serial.print("Drawed lines\n");
  display.display();
  delay(5000);
*/
  
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0340 / 2;
  if (distance <= 40)
  {
    reader();
  }
  delay(1000);
  
}

void reader()
{
  
if(Serial.available())// Check if there is incoming data in the RFID Reader Serial Buffer.
  {
    count = 0; // Reset the counter to zero
    while(Serial.available() && count < 12) 
    {
      input[count] = Serial.read(); // Read 1 Byte of data and store it in the input[] variable
      count++; // increment counter
      delay(5);
    }
    if(count == 12) // 
    {
      count =0; // reset counter varibale to 0
      flag = 1;
      while(count<12 && flag !=0)  
      {
        if(input[count]==tag[count])
        flag = 1; // everytime the values match, we set the flag variable to 1
        else
        flag= 0; 
        count++; // increment i
        RfidReading = "Ashish";
      }
    }
    if(flag == 1) // If flag variable is 1, then it means the tags match
    {
      // Serial.println("Access Allowed!");
      temp_read();
      }
    else
    {
     Serial.println("Access Denied"); // Incorrect Tag Message     
    }
    for(count=0; count<12; count++) 
    {
      input[count]= 'F';
    }
    count = 0; // Reset counter variable
    
  }
}


void temp_read()
{
   TempReading = mlx.readObjectTempC();
   
     Serial.println(sensorReading1);
     Serial.print(",");
    Serial.print("Ambient ");
    Serial.print(mlx.readAmbientTempC());
    Serial.print(" C");
     Serial.print("Target  ");
     Serial.print(mlx.readObjectTempC());
     Serial.print(" C");
     delay(1000);
}

#endif // MAIN // -----------------------------------------------------------------------------------------------



// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// -------------------------------------------- UTILS -------------------------------------------- 
// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
#ifdef I2C_SCANNER
#include <Wire.h>
 
void setup()
{
    Wire.begin();
 
    Serial.begin(9600);
    while (!Serial); // Wait for serial monitor
    Serial.println("---I2C Scanner---");
}
 
void loop()
{
    byte error, address;
    int nDevices;
 
    Serial.println("Scanning...");
 
    nDevices = 0;
    for(address = 1; address < 127; address++ )
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
 
        Wire.beginTransmission(address+1);
 
    if (error == 0 && Wire.endTransmission() != 0 ) // Special flag for SAMD Series
    {
        Serial.print("I2C device found at address 0x");
        if (address<16)
            Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("!");
 
        nDevices++;
    }
    else if (error==4) 
    {
        Serial.print("Unknown error at address 0x");
        if (address<16) 
            Serial.print("0");
        Serial.println(address,HEX);
    }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
 
    delay(5000);           // wait 5 seconds for next scan
}
#endif // I2C_SCANNER

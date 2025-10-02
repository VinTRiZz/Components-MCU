
#include <Wire.h>
#include "Adafruit_MLX90614.h"
MLX90614 mlx = AMLX90614();

char tag[] ="180088FECCA2"; // Replace with your own Tag ID
char input[12];        // A variable to store the Tag ID being presented
int count = 0;        // A counter variable to navigate through the input[] character array
boolean flag = 0;     // A variable to store the Tag match status

const int trigPin = 5;
const int echoPin = 6;

long duration = 0;
int distance = 0; 
String RfidReading = 0;
float TempReading = 0;
void setup()
{
  Serial.begin(9600);   // Initialise Serial Communication with the Serial Monitor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  mlx.begin();
  Initialize_streamer();
}
void loop()
{ 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0340 / 2;
     //Serial.println("Distance");
     //Serial.println(distance);
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
      //Serial.println("Access Allowed!");
      temp_read();
      Write_streamer();
      }
    else
    {
     // Serial.println("Access Denied"); // Incorrect Tag Message     
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
   
  // Serial.println(sensorReading1);
  // Serial.print(",");
 //Serial.print("Ambient ");
 //Serial.print(mlx.readAmbientTempC());
 //Serial.print(" C");
// Serial.print("Target  ");
// Serial.print(mlx.readObjectTempC());
// Serial.print(" C");
// delay(1000);
}

void Initialize_streamer()
{
  Serial.println("CLEARDATA"); //clears up any data left from previous projects
  Serial.println("LABEL,Date,Time,Temperature,Name"); //always write LABEL, to indicate it as first line
}
void Write_streamer()
{
   // Serial.print("DATA"); //always write "DATA" to Indicate the following as Data
   // Serial.print(","); //Move to next column using a ","
  //  Serial.print("DATE"); //Store date on Excel
   // Serial.print(","); //Move to next column using a ","
   // Serial.print("TIME"); //Store date on Excel
   // Serial.print(","); //Move to next column using a ","
   
    Serial.print(RfidReading); //Store date on Excel
    Serial.print(","); //Move to next column using a ","
    Serial.print(TempReading); //Store date on Excel
    Serial.print(","); //Move to next column using a ","
    Serial.println(); //End of Row move to next row
}




/******************
// Previous version
const int trigPin = 5;
const int echoPin = 6;


long duration = 0;
int distance = 0;
String RfidReading = 0;
float TempReading = 0;


void setup()
{
  Serial.begin(9600);   // Initialise Serial Communication with the Serial Monitor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  mlx.begin();
  Initialize_streamer();
}

void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0340 / 2;
  if (distance <= 25){
  reader();  
}

void reader()
{
  if(Serial.available())
    {
      count = 0;
      while(Serial.available() && count < 12)
      {
        input[count] = Serial.read();
        count++;
        delay(5);
        
        if(input[count]==tag[count])
          flag = 1;        
        else
          flag= 0;
        
        count++;        
        RfidReading = "Ashish";
        }
      }
      if(flag == 1)
      {
        temp_read();
        Write_streamer();
      }
}
      
void temp_read()
{
   TempReading = mlx.readObjectTempC();
}
*******************/

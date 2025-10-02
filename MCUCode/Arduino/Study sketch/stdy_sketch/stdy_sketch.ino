// Defines
#define LED_13 13

// First of all
void setup()
{
  pinMode(LED_13, OUTPUT);      // sets the digital pin as output or input
}

void loop()
{
  digitalWrite(LED_13 HIGH); // Set pin or unset
  delay(1000);
}





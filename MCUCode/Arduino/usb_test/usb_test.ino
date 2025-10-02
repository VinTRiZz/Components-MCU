void setup() {
  // put your setup code here, to run once:
  Serial.begin(460800);
}

char buf = 'E';

void loop() {

  Serial.print("Start!\n");

  /*
  for (int i = 0; buf && i < 10; i++)
  {
    #warning "WTF WITH READ IN ARDUINO?"
    // buf = Serial.peek(); 
    Serial.print(buf);
  }
  Serial.println();
  */
  /*
  if (Serial.available() > 0)
  {
      Serial.println("Read data:");
      
      Serial.println((char)Serial.read());
  }*/

  delay(500);
  Serial.print("End!\n");
  delay(1000);
}

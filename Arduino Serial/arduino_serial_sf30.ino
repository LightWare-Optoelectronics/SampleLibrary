#include <SoftwareSerial.h>

SoftwareSerial rangefinder(10, 11);

void setup() {
  Serial.begin(57600);
  rangefinder.begin(19200);
  rangefinder.stopListening();
}

float takeReading()
{
  rangefinder.listen();  
  
  while (rangefinder.available() < 2);
      
  int byteL = rangefinder.read();
  int byteH = rangefinder.read();
  float distance = byteH + byteL / 256.0;

  rangefinder.stopListening();

  return distance;  
}

void loop() {
  Serial.print(takeReading());
  Serial.println("m");
  delay(645);
}
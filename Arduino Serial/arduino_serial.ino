#include <SoftwareSerial.h>

SoftwareSerial rangefinder(10, 11);

void setup() {
  Serial.begin(57600);
  rangefinder.begin(19200);
}

void loop() {
  char resultBuffer[8];
  int bufferIndex = 0;
  int c = 0;
  
  rangefinder.write('d');
  
  while(c != '\n')
  {
    while (!rangefinder.available());
    c = rangefinder.read();
    resultBuffer[bufferIndex++] = c;
  }

  resultBuffer[bufferIndex - 2] = 0;
  float distance = atof(resultBuffer);
  
  Serial.print(distance);
  Serial.println("m");
  
  delay(100);  
}
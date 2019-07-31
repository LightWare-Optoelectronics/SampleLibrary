//------------------------------------------------------------------------
// LightWare 2019
//------------------------------------------------------------------------
// Sample code for interfacing with the SF11 over serial UART with 
// the Arduino.
//------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  // Check what baud rate the SF11 is using for serial communication in the SF11 settings menu. (Communication menu -> Serial port baud rate)
  Serial1.begin(115200);

  // Flush communication with the SF11.
  Serial.write("\r\n");
  while (Serial1.available()) {
    Serial1.read();
  }
}

void loop() {
  char resultBuffer[8];
  int bufferIndex = 0;
  int c = 0;
  
  // Request a distance.
  Serial1.write("d\n");
  
  // Wait for distance response.
  while(c != '\n') {
    while (!Serial1.available());
    c = Serial1.read();
    
    resultBuffer[bufferIndex++] = c;
  }

  // Process response into distance value.
  resultBuffer[bufferIndex - 2] = 0;
  float distance = atof(resultBuffer);
  
  Serial.print(distance);
  Serial.println(" m");
  
  delay(100);  
}

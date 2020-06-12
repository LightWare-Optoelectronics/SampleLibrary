//-------------------------------------------------------------------------------------------
// LightWare Arduino serial connection sample.
// https://lightware.co.za
//-------------------------------------------------------------------------------------------
// Compatible with:
// - SF02
// - SF10
// - SF11
// - LW20/SF20
//-------------------------------------------------------------------------------------------
// Please note that this sample needs an Arduino board that supports at least 2 hardware
// UARTs. If you are using an Uno you can use the software serial library instead of the
// Serial1 hardware UART.
//-------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  // Baud rate of the device can be configured.
  Serial1.begin(115200);

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

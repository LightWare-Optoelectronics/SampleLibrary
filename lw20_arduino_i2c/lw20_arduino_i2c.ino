//-------------------------------------------------------------------------------------------
// LightWare Arduino LW20/SF20 I2C connection sample.
// https://lightware.co.za
//-------------------------------------------------------------------------------------------
// This sample is compatible with any version of the LW20/SF20 and uses the MMI protcol.
//-------------------------------------------------------------------------------------------

#include <Wire.h>

const int i2cAddress = 0x66;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Starting...");

  // Make sure I2C protocol is active
  Wire.requestFrom(i2cAddress, 0);

  // Get product info.
  Wire.beginTransmission(i2cAddress);
  Wire.write('?');
  Wire.write('\r');
  Wire.endTransmission(true);

  delay(10);

  Wire.requestFrom(i2cAddress, 16);
  
  char strBuf[16];

  for (int i = 0; i < 16;++i) {
    strBuf[i] = Wire.read();
  }

  Serial.print('[');
  Serial.print(strBuf);
  Serial.print(']');
  
  Serial.println();
  
  Serial.println("Setup completed");
}

void loop() {
  Wire.beginTransmission(i2cAddress);
  Wire.write('?');
  Wire.write('l');
  Wire.write('d');
  Wire.write('f');
  Wire.write(',');
  Wire.write('1');
  Wire.write('\r');
  Wire.endTransmission(true);

  delay(0);

  Wire.requestFrom(i2cAddress, 16);
  
  char strBuf[16];

  for (int i = 0; i < 16;++i) {
    strBuf[i] = Wire.read();
  }

  Serial.print(strBuf + 6);
  
  Serial.println();

  delay(20);
}

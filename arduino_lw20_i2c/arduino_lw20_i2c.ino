#include <Wire.h>

const int i2cAddress = 0x66;

void readDistanceOutputValue() {
  Wire.beginTransmission(i2cAddress);
  Wire.write(27);    // Register
  Wire.endTransmission(false);
  Wire.requestFrom(i2cAddress, 4);
  uint32_t distanceOutput = ((uint32_t)Wire.read() << 0) | ((uint32_t)Wire.read() << 8) | ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 24);
  
  Serial.print("Distance output: ");
  Serial.println(distanceOutput);
}

void setDistanceOutputValue(uint32_t Value) {
  Wire.beginTransmission(i2cAddress);
  Wire.write(27);    // Register
  
  Serial.println((Value >> 0) & 0xFF);
  Serial.println((Value >> 8) & 0xFF);
  Serial.println((Value >> 16) & 0xFF);
  Serial.println((Value >> 24) & 0xFF);
  
  Wire.write((Value >> 0) & 0xFF);
  Wire.write((Value >> 8) & 0xFF);
  Wire.write((Value >> 16) & 0xFF);
  Wire.write((Value >> 24) & 0xFF);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Starting...");

  //-----------------------------------------------------------------------
  // Make sure I2C protocol is active
  //-----------------------------------------------------------------------
  Wire.requestFrom(i2cAddress, 0);

  //-----------------------------------------------------------------------
  // Get product info.
  //-----------------------------------------------------------------------
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

//  Serial.print('[');
  Serial.print(strBuf + 6);
//  Serial.print(']');
  
  Serial.println();

  delay(20);
}

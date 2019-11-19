//-------------------------------------------------------------------------------------------
// LightWare Arduino I2C connection sample
// https://lightware.co.za
//-------------------------------------------------------------------------------------------
// This sample uses the alternate binary protocol (LWNX) over I2C. This is compatible with
// any LW20/SF20 with hardware revision 11 or more.
//-------------------------------------------------------------------------------------------

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
  // Enable register based protocol.
  //-----------------------------------------------------------------------
  Wire.beginTransmission(i2cAddress);
  Wire.write(120);    // Register
  Wire.write(0xAA);   // Byte 1
  Wire.write(0xAA);   // Byte 2
  Wire.endTransmission();

  //-----------------------------------------------------------------------
  // Check register protocol activated.
  //-----------------------------------------------------------------------
  Wire.beginTransmission(i2cAddress);
  Wire.write(120);    // Register
  Wire.endTransmission(false);
  Wire.requestFrom(i2cAddress, 2);
  byte a = Wire.read();
  byte b = Wire.read();

  Serial.print("Register mode response:");
  Serial.print(a);
  Serial.print(" ");
  Serial.println(b);

  if (a != 0xCC && b != 0) {
    Serial.println("Failed to activate register mode");
    return;
  }

  //-----------------------------------------------------------------------
  // Check data output mode.
  //-----------------------------------------------------------------------
  readDistanceOutputValue();
  setDistanceOutputValue(561);
  readDistanceOutputValue();

  Serial.println("Setup completed");
}

void loop() {
  Wire.beginTransmission(i2cAddress);
  Wire.write(44);    // Register
  Wire.endTransmission(false);
  Wire.requestFrom(i2cAddress, 8);
  int16_t first = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
  int16_t firstStrength = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
  int16_t last = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
  int16_t lastStrength = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
  
  Serial.print(first);
  Serial.print(" ");
  Serial.print(last);
  Serial.print(" ");
  Serial.print(firstStrength);
  Serial.print(" ");
  Serial.println(lastStrength);
  delay(100);
}

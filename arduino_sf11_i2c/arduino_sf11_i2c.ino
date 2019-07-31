//---------------------------------------------------------
// LightWare Arduino I2C Connection Sample
//---------------------------------------------------------
#include <Wire.h>

void setup() {
  Serial.begin(115200);
	Wire.begin();
}

void loop() {
	// Get a 2 byte reading from the SF11.
	// This is using address 0x66 which can be changed in the SF11 settings.
	Wire.requestFrom(0x66, 2);
	
	if (Wire.available() >= 2) {
		int byteH = Wire.read();
		int byteL = Wire.read();
		int distanceInCM = byteH * 256 + byteL;
		
    Serial.print(distanceInCM);
    Serial.println(" cm");
	}
	
	delay(100);
}

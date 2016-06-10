//---------------------------------------------------------
// LightWare Arduino I2C Connection Sample
//---------------------------------------------------------

#include <Wire.h>

void setup()
{
	Wire.begin();
	Serial.begin(9600);
}

void loop()
{
	// Get a 2 byte reading from the SF02.
	// This is using address 0x55 which can be changed in the SF02 settings.
	Wire.requestFrom(0x55, 2);
	
	if (Wire.available() >= 2)
	{
		int byteH = Wire.read();
		int byteL = Wire.read();
		int distanceInCM = byteH * 256 + byteL;
		
		Serial.println(distanceInCM);
	}
	
	delay(100);
}
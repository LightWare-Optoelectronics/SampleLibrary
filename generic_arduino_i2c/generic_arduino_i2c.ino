//-------------------------------------------------------------------------------------------
// LightWare Arduino I2C connection sample
// https://lightware.co.za
//-------------------------------------------------------------------------------------------
// Compatible with the following devices:
// - SF02
// - SF10
// - SF11
// - LW20/SF20
//-------------------------------------------------------------------------------------------

#include <Wire.h>

void setup()
{
	Wire.begin();
	Serial.begin(115200);
}

void loop()
{
	// This is using address 0x6 which can be changed in the device settings.
	Wire.requestFrom(0x66, 2);
	
	if (Wire.available() >= 2)
	{
		int byteH = Wire.read();
		int byteL = Wire.read();
		int distanceInCM = byteH * 256 + byteL;
		
		Serial.print(distanceInCM);
		Serial.println(" cm");
	}
	
	delay(100);
}
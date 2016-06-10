/*
  LightWare SF30 Serial Sample
  
NOTE:
-This demo assumes that the SF30 is set to a UART speed of 38400 Baud.
-This demo assumes that the SF30 is set to a 0.03m Snapshot Resolution. 
-This demo assumes that the SF30 is set to a 1144/sec Serial Port Update Rate.

Wiring Diagram:

Pin_GND (Black)  On SF30 Laser Rangefinder - GND (Vss On Some Boards)
Pin_+5V (Red)    On SF30 Laser Rangefinder - +5V (Vdd On Some Boards)
Pin_TXD (Yellow) On SF30 Laser Rangefinder - Arduino RX Pin (10)
Pin_RXD (Orange) On SF30 Laser Rangefinder - Arduino TX Pin (11)
  
*/

#include <SoftwareSerial.h>

#define terminal_baud_rate    115200                                   // terminal baud rate 115200
#define serial_Rxd               10                                    // These pin definitions can be changed to match your wiring
#define serial_Txd               11
#define sf30_baud_rate         38400                                   // Sf30 baud rate  (Can be changed, refer to the sf30 manual)

SoftwareSerial sf30_serial(serial_Rxd, serial_Txd);                    // The pins used for the second serial port


float distance;                                                        // The Laser Range Finder Distance Variable
int Byte_L, Byte_H;

void setup()                                          
{
  Serial.begin(terminal_baud_rate);                                     // Open the main USB serial port on the Arduino ready for the terminal application
  while (!Serial);                                                      // Wait for serial port to connect.
  sf30_serial.begin(sf30_baud_rate);                                    // Open the second serial port to connect to the sf30
}

void loop()
{  
    //sf30_serial.available();
	// Sync line with interrupt??
	
	// Either the SF30Serial or SoftwareSerial can be active.
	// stopListening();
	
	// SF30 option that is a flat blocking interface and only enables reading interrupts directly
	// when querried for a result.
	
	SF30Serial sf30(rxPin, syncPin);
	
	// Non-blocking read cycle
	sf30.startMeasurement();
	while (!sf30.isMeasurementComplete());
	distance = sf30.getMeasurement();
	
	// Waits for and reads a new distance.
	distance = sf30.readDistance();
	
	// IDEA: Use interrupt trigger time to determine if start byte or what.
	
	//attachInterrupt(0,MyLib.MyFunction,RISING);
	
	// Old incorrect code
	while (sf30_serial.available() >= 2)                                 // Wait here for the next character
    {
		Byte_H = sf30_serial.read();                                        // Store the byte into Byte_H
		Byte_L = sf30_serial.read();                                        // Store the byte into Byte_L
		distance = (float(Byte_L))/256 + Byte_H;                            // Get distance from the bytes
		Serial.print("Distance in meters = ");                              // Print "Distance in meters = "
		Serial.println(distance, 4);                                        // Print values, then go to a new lines
		delay(10);                                                          // Pause .1 sec
		sf30_serial.available();                                            // Check buffer if byte available
    }
}

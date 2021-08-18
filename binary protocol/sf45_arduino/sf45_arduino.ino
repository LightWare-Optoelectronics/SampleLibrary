#include "lwNx.h"

lwSerialPort devicePort;

void exitCommandFailure() {
	Serial.println("Failed to get a response from the Sf45.");
}

void setup() {
	platformInit();

	// Connect to serial monitor.
	Serial.begin(115200);
	Serial.println("Hello");

	// Connect to SF45.
	Serial1.begin(115200);
	devicePort.connect(&Serial1, 115200);
	Serial1.flush();

	// NOTE: Find descriptions of each command here http://support.lightware.co.za/sf45b/#/commands

	// Read the product name. (Command 0: Product name)
	char modelName[16];
	if (!lwnxCmdReadString(&devicePort, 0, modelName)) { exitCommandFailure(); return; }
	
	// Read the hardware version. (Command 1: Hardware version)
	uint32_t hardwareVersion;
	if (!lwnxCmdReadUInt32(&devicePort, 1, &hardwareVersion)) { exitCommandFailure(); return; }
	
	// Read the firmware version. (Command 2: Firmware version)
	uint32_t firmwareVersion;
	if (!lwnxCmdReadUInt32(&devicePort, 2, &firmwareVersion)) { exitCommandFailure(); return; }
	char firmwareVersionStr[16];
	lwnxConvertFirmwareVersionToStr(firmwareVersion, firmwareVersionStr);

	// Read the serial number. (Command 3: Serial number)
	char serialNumber[16];
	if (!lwnxCmdReadString(&devicePort, 3, serialNumber)) { exitCommandFailure(); return; }

	Serial.print("Model: "); Serial.println(modelName);
	Serial.print("Hardware: "); Serial.println(hardwareVersion);
	Serial.print("Firmware: "); Serial.println(firmwareVersionStr);
	Serial.print("Serial: "); Serial.println(serialNumber);
}

void loop() {

}

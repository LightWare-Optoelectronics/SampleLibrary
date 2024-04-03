#include "lwNx.h"

lwSerialPort devicePort;

void exitCommandFailure() {
	Serial.println("Failed to get a response from the SF45.");
}

uint16_t readInt16(uint8_t* Buffer, uint32_t Offset) {
	uint16_t result;
	result = (Buffer[Offset + 0] << 0) | (Buffer[Offset + 1] << 8);
	return result;
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

	// Set the output rate to 50 readings per second. (Command 66: Update rate)
	if (!lwnxCmdWriteUInt8(&devicePort, 66, 1)) { exitCommandFailure(); return; }

	// Set distance output to include the following: (Command 27: Distance output)
	// first return raw distance: 0
	// first return strength: 2
	// temperature: 7
	// yaw angle: 8
	if (!lwnxCmdWriteUInt32(&devicePort, 27, 0x185)) { exitCommandFailure(); return; }
	
	// Enable streaming of point data. (Command 30: Stream)
	if (!lwnxCmdWriteUInt32(&devicePort, 30, 5)) { exitCommandFailure(); return; }

	// Continuously wait for and process the streamed point data packets.
	// The incoming point data packet is Command 44: Distance data in cm.
	while (1) {
		lwResponsePacket response;
		
		if (lwnxRecvPacket(&devicePort, 44, &response, 1000)) {
			// NOTE: There is a 4 byte offset to account for the packet header.
			uint16_t firstReturnRaw = readInt16(response.data, 4);
			uint16_t firstReturnStrength = readInt16(response.data, 6);
			float temperature = readInt16(response.data, 8) / 100.0;
			float yawAngle = (((int16_t)readInt16(response.data, 10)) / 100.0);

			Serial.print("Distance: ");
			Serial.print(firstReturnRaw);
			Serial.print(" cm  Strength: ");
			Serial.print(firstReturnStrength);
			Serial.print(" %  Temperature: ");
			Serial.print(temperature);
			Serial.print(" degrees  Angle: ");
			Serial.print(yawAngle);
			Serial.println(" degrees");
		}
	}
}

void loop() {

}

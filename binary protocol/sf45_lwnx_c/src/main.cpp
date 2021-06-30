//----------------------------------------------------------------------------------------------------------------------------------
// LightWare LWNX Example.
//----------------------------------------------------------------------------------------------------------------------------------
#include "common.h"
#include "lwNx.h"

//----------------------------------------------------------------------------------------------------------------------------------
// Helper utilities.
//----------------------------------------------------------------------------------------------------------------------------------
void printHexDebug(uint8_t* Data, uint32_t Size) {
	printf("Buffer: ");

	for (int i = 0; i < Size; ++i) {
		printf("0x%02X ", Data[i]);
	}

	printf("\n");
}

void exitWithMessage(const char* Msg) {
	printf("%s\nPress any key to Exit...\n", Msg);
	std::cin.ignore();
	exit(1);
}

void exitCommandFailure() {
	exitWithMessage("No response to command, terminating sample.\n");
}

uint16_t readInt16(uint8_t* Buffer, uint32_t Offset) {
	uint16_t result;
	result = (Buffer[Offset + 0] << 0) | (Buffer[Offset + 1] << 8);
	return result;
}

//----------------------------------------------------------------------------------------------------------------------------------
// Application Entry.
//----------------------------------------------------------------------------------------------------------------------------------
int main(int args, char **argv)
{
	platformInit();
	
	printf("LWNX sample\n");

	// NOTE: Change the port name to the one assigned by the OS to the plugged in lidar.
#ifdef __linux__
	const char* portName = "/dev/ttyUSB0";
#else
	const char* portName = "\\\\.\\COM10";
#endif

	int32_t baudRate = 921600;

	lwSerialPort* serial = platformCreateSerialPort();
	if (!serial->connect(portName, baudRate)) {
		exitWithMessage("Could not establish serial connection\n");
	};

	// NOTE: Find descriptions of each command here http://support.lightware.co.za/sf45b/#/commands

	// Read the product name. (Command 0: Product name)
	char modelName[16];
	if (!lwnxCmdReadString(serial, 0, modelName)) { exitCommandFailure(); }

	// Read the hardware version. (Command 1: Hardware version)
	uint32_t hardwareVersion;
	if (!lwnxCmdReadUInt32(serial, 1, &hardwareVersion)) { exitCommandFailure(); }

	// Read the firmware version. (Command 2: Firmware version)
	uint32_t firmwareVersion;	
	if (!lwnxCmdReadUInt32(serial, 2, &firmwareVersion)) { exitCommandFailure(); }
	char firmwareVersionStr[16];
	lwnxConvertFirmwareVersionToStr(firmwareVersion, firmwareVersionStr);

	// Read the serial number. (Command 3: Serial number)
	char serialNumber[16];
	if (!lwnxCmdReadString(serial, 3, serialNumber)) { exitCommandFailure(); }

	printf("Model: %.16s\n", modelName);
	printf("Hardware: %d\n", hardwareVersion);
	printf("Firmware: %.16s (%d)\n", firmwareVersionStr, firmwareVersion);
	printf("Serial: %.16s\n", serialNumber);

	// Set the output rate to 500 readings per second. (Command 66: Update rate)
	if (!lwnxCmdWriteUInt8(serial, 66, 5)) { exitCommandFailure(); }

	// Set distance output to include the following: (Command 27: Distance output)
	// first return raw distance: 0
	// first return strength: 2
	// temperature: 7
	// yaw angle: 8
	if (!lwnxCmdWriteUInt32(serial, 27, 0x185)) { exitCommandFailure(); }
	
	// Enable streaming of point data. (Command 30: Stream)
	if (!lwnxCmdWriteUInt32(serial, 30, 5)) { exitCommandFailure(); }

	// Continuously wait for and process the streamed point data packets.
	// The incoming point data packet is Command 44: Distance data in cm.
	while (1) {
		lwResponsePacket response;
		
		if (lwnxRecvPacket(serial, 44, &response, 1000)) {
			// NOTE: There is a 4 byte offset to account for the packet header.
			uint16_t firstReturnRaw = readInt16(response.data, 4);
			uint16_t firstReturnStrength = readInt16(response.data, 6);
			float temperature = readInt16(response.data, 8) / 100.0;
			float yawAngle = readInt16(response.data, 10) / 100.0;

			printf("Distance: %5d cm  Strength: %5d %%  Temperature: %f degrees  Angle: %f degrees\n", firstReturnRaw, firstReturnStrength, temperature, yawAngle);
		}
	}

	return 0;
}
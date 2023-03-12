//----------------------------------------------------------------------------------------------------------------------------------
// LightWare SF30 USB Example.
//----------------------------------------------------------------------------------------------------------------------------------
#include "common.h"

//----------------------------------------------------------------------------------------------------------------------------------
// Helper utilities.
//----------------------------------------------------------------------------------------------------------------------------------
void exitWithMessage(const char* Msg) {
	printf("%s\nPress any key to Exit...\n", Msg);
	std::cin.ignore();
	exit(1);
}

float getNextReading(lwSerialPort* Port) {
	char line[64];
	int lineSize = 0;

	while (1) {
		char recvData;
		Port->readData((uint8_t*)&recvData, 1);

		if (recvData == '\n') {
			line[lineSize] = 0;
			float distance = atof(line);
			return distance;
		} else if (recvData != '\r' && recvData != ' ') {
			line[lineSize++] = recvData;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
// Application Entry.
//----------------------------------------------------------------------------------------------------------------------------------
int main(int args, char **argv)
{
	platformInit();
	
	printf("SF30 USB sample\n");

	// NOTE: Change the port name to the one assigned by the OS to the plugged in lidar.
#ifdef __linux__
	const char* portName = "/dev/ttyUSB0";
#else
	const char* portName = "\\\\.\\COM3";
#endif

	// NOTE: The baudrate is ignored when using USB.
	int32_t baudRate = 921600;

	lwSerialPort* serial = platformCreateSerialPort();
	if (!serial->connect(portName, baudRate)) {
		exitWithMessage("Could not establish serial connection\n");
	};

	// NOTE: The SF30 needs to be configured with a teminal program before being used by this code.
	// The 'Output type' or 'Active data port' must be set to one of the USB options.
	// The 'USB port output rate' can be configured as required.
	
	// (If you have just configured the SF30 with a terminal program, please disconnect and reconnect the USB port
	// before running this code.)
	
	// Continuously wait for and process the distance data.
	while (1) {
		float distanceMeters = getNextReading(serial);
		printf("Distance: %.2f m\n", distanceMeters);
	}

	return 0;
}
//-------------------------------------------------------------------------
// LightWare SF23 Linux Example
//-------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include "time.h"

#include "lwnx.h"

//-------------------------------------------------------------------------
// Platform Implementation.
// The platform used here is Linux, but any platform can be used by
// replacing the contents of these functions.
//-------------------------------------------------------------------------
int g_serialPortFd = -1;

// Get the time in milliseconds from the system. Does not need to start at 0.
int32_t getTimeMilliseconds() {
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);

	return (time.tv_sec * 1000000 + time.tv_nsec / 1000) / 1000;
}

// Initiate a serial port connection.
int portConnect(const char* Name, int BitRate) {
	g_serialPortFd = -1;
	printf("Attempt com connection: %s\n", Name);
		
	g_serialPortFd = open(Name, O_RDWR | O_NOCTTY | O_SYNC);
	
	if (g_serialPortFd < 0) {
		printf("Couldn't open serial port!\n");
		return -1;
	}

	struct termios tty;
	memset(&tty, 0, sizeof(tty));
	if (tcgetattr(g_serialPortFd, &tty) != 0) {
		printf("Error from tcgetattr\n");
		return -1;
	}

	cfsetospeed(&tty, BitRate);
	cfsetispeed(&tty, BitRate);

	tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 1;

	if (tcsetattr(g_serialPortFd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr\n");
		return -1;
	}

	printf("Connected\n");
	
	return g_serialPortFd;
}

// Write BufferSize bytes to the serial port from Buffer.
int portWrite(uint8_t* Buffer, int32_t BufferSize) {
	if (g_serialPortFd < 0) {
		printf("Serial port not connected.\n");
		return -1;
	}

	int writtenBytes = write(g_serialPortFd, Buffer, BufferSize);

	if (writtenBytes != BufferSize)	{
		printf("Could not send all bytes! (%d)\n", writtenBytes);
		return writtenBytes;
	}

	return writtenBytes;
}

// Read up to BufferSize bytes from the serial port into Buffer.
int portRead(uint8_t* Buffer, int32_t BufferSize) {
	if (g_serialPortFd < 0) {
		printf("Serial port not connected.\n");
		return -1;
	}

	int readBytes = read(g_serialPortFd, Buffer, BufferSize);
	
	return readBytes;
}

//-------------------------------------------------------------------------
// Read version information.
//-------------------------------------------------------------------------
void readProductInformation(lwEndpoint* endpoint) {
	printf("Getting product information...\n");

	// Read the hardware model. (Command 0: Hardware model)
	char modelName[16] = {};
	lwnxCmdReadString(endpoint, 0, modelName);
	printf("Hardware model: %s\n", modelName);

	// Read the hardware version. (Command 1: Hardware version)
	uint32_t hardwareVersion = 0;
	lwnxCmdReadUInt32(endpoint, 1, &hardwareVersion);
	printf("Hardware version: %u\n", hardwareVersion);

	// Read the firmware version. (Command 2: Firmware version)
	uint8_t firmwareVersion[4] = {};
	lwnxCmdReadData(endpoint, 2, firmwareVersion, 4);
	printf("Firmware version: %u.%u.%u\n", firmwareVersion[2], firmwareVersion[1], firmwareVersion[0]);

	// Read the serial number. (Command 3: Serial)
	char serialNumber[16] = {};
	lwnxCmdReadString(endpoint, 3, serialNumber);
	printf("Serial number: %s\n", serialNumber);
}

//-------------------------------------------------------------------------
// Perform upgrade.
//-------------------------------------------------------------------------
void performUpgrade(lwEndpoint* endpoint) {
	// Load bytes from upgrade file and get the size of the file.
	// FILE* fd = fopen("./upgrade_sf23_r2_1.0.4.lwf", "rb");
	FILE* fd = fopen("./upgrade_sf23_r2_1.0.5.lwf", "rb");

	if (fd == 0) {
		printf("Could not open upgrade file\n");
		return;
	}

	fseek(fd, 0, SEEK_END);
    long fileSize = ftell(fd);

	int32_t pageCount = fileSize / 128;

	printf("Upgrading - file size: %ld page count: %d\n", fileSize, pageCount);

	typedef struct {
		uint16_t pageIndex;
		uint8_t pageData[128];

	} lwUploadFirmwareRequest;

	// Upload all firmware pages.
	for (int32_t i = 0; i < pageCount; ++i) {
		lwUploadFirmwareRequest request = {};
		request.pageIndex = i;
		fseek(fd, i * 128, SEEK_SET);
		
		if (fread(request.pageData, 128, 1, fd) != 1) {
			printf("Could not read from file\n");
			return;
		}

		// Write the firmware page. (Command 16: Upload firmware page)
		int32_t response = 0;
		lwnxHandleManagedCmd(endpoint, 16, (uint8_t*)&response, 4, 1, (uint8_t*)&request, 130);
		printf("Uploaded page %d - response: %d\n", i, response);

		if (response != i) {
			printf("Upgrade failed while uploading page %d with error: %d\n", i, response);
			return;
		}
	}

	// Commit the new firmware. (Command 17: Finalize new firmware)
	int32_t response = 0;
	lwnxHandleManagedCmd(endpoint, 17, (uint8_t*)&response, 4, 1, 0, 0);
	printf("Commit reponse: %d\n", response);

	if (response != 1) {
		printf("Upgrade failed while commiting, integrity check failed.\n");
		return;
	}

	printf("Restarting...\n");

	// Read the current token. (Command 10: Token)
	uint16_t token = 0;
	lwnxCmdReadInt16(endpoint, 10, &token);

	// Initiate a processor reset. (Command 14: Reset)
	lwnxCmdWriteInt16(endpoint, 14, token);

	// Wait for restart to complete.	
	int32_t timeout = getTimeMilliseconds() + 2000;

	while (getTimeMilliseconds() < timeout);

	printf("Upgrade complete\n");
}

//-------------------------------------------------------------------------
// Main application.
//-------------------------------------------------------------------------
int main(int args, char** argv)
{
	printf("SF23 LWNX sample\n");

	portConnect("/dev/ttyUSB0", B921600);

	// Setup serial port callbacks so the LWNX protocol can read/write data through the serial port.
	lwEndpoint endpoint = {};
	endpoint.writeCallback = portWrite;
	endpoint.readCallback = portRead;
	endpoint.timeCallback = getTimeMilliseconds;

	readProductInformation(&endpoint);
	performUpgrade(&endpoint);
	readProductInformation(&endpoint);

	return 0;
}
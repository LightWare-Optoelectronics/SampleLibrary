//-------------------------------------------------------------------------------------------
// LightWare Linux LW20/SF20 I2C connection and upgrade sample.
// https://lightwarelidar.com
//-------------------------------------------------------------------------------------------

// Compile with:
// gcc -O3 main.cpp -o main

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "i2clib.h"

//-------------------------------------------------------------------------
// Perform upgrade.
//-------------------------------------------------------------------------
int performUpgrade(int i2cFd, int address, const char* upgradeFilepath) {
	int result = 0;
	int32_t response = 0;

	// Load bytes from upgrade file and get the size of the file.
	FILE* fd = fopen(upgradeFilepath, "rb");

	if (fd == 0) {
		printf("Failed to open upgrade file\n");
		return -1;
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
			printf("Failed to read from file\n");
			return -1;
		}

		// Write the firmware page. (Command 16: Upload firmware page)
		if (writeReg(i2cFd, address, 16, (uint8_t*)&request, 130) < 0) {
			printf("Failed to write device register\n");
			return -1;
		}
		if (readReg(i2cFd, address, 16, (uint8_t*)&response, 4) < 0) {
			printf("Failed to read device register\n");
			return -1;
		}
		
		printf("Uploaded page %d/%d - response: %d\n", i, pageCount - 1, response);

		if (response != i) {
			printf("Upgrade failed while uploading page %d with error: %d\n", i, response);
			return -1;
		}
	}

	// Commit the new firmware. (Command 17: Finalize new firmware)
	uint8_t temp = 0;
	result = writeReg(i2cFd, address, 17, &temp, 1);
	result = readReg(i2cFd, address, 17, (uint8_t*)&response, 4);

	printf("Commit response: %d\n", response);

	if (response != 1) {
		printf("Upgrade failed while commiting, integrity check failed.\n");
		return -1;
	}

	// Read the current token. (Command 10: Token)
	uint16_t token = 0;
	if (readReg(i2cFd, address, 10, (uint8_t*)&token, 2)) {
		printf("Failed to read device register\n");
		return -1;
	}
	
	// Initiate a processor reset. (Command 14: Reset)
	printf("Restarting...\n");

	if (writeReg(i2cFd, address, 14, (uint8_t*)&token, 2) < 0) {
		printf("Failed to write device register\n");
		return -1;
	}

	// Wait for restart to complete.	
	sleep(2);

	printf("Upgrade complete\n");

	return 0;
}

int main(int argc, char **argv) {
	//-----------------------------------------------------------------------
	// Connect to I2C device.
	//-----------------------------------------------------------------------
	int deviceAddress = 0x66;
	int fd = i2cOpenDevice("/dev/i2c-1");

	if (fd < 0) {
		printf("Failed to open I2C device file\n");
		exit(1);
	}

	printf("I2C Bus opened on FD: %d\n", fd);

	//-----------------------------------------------------------------------
	// Wake device and enable register based protocol.
	//-----------------------------------------------------------------------
	uint16_t cmd = 0xAAAA;

	if (writeReg(fd, deviceAddress, 120, (uint8_t*)&cmd, 2) < 0) {
		// NOTE: This is written twice because the first time may only wake the i2c bus of the unit.
		sleep(1);

		if (writeReg(fd, deviceAddress, 120, (uint8_t*)&cmd, 2) < 0) {
			printf("Failed to write device register\n");
			exit(1);
		}
	}
	
	//-----------------------------------------------------------------------
	// Check register protocol activated.
	//-----------------------------------------------------------------------
	uint16_t regResult = 0;
	if (readReg(fd, deviceAddress, 120, (uint8_t*)&regResult, 2) < 0) {
		printf("Failed to read device register\n");
		exit(1);
	}
	
	if (regResult != 0xCC) {
		printf("Failed to activate register mode\n");
		exit(1);
	}

	printf("Register protocol mode activated\n");

	//-----------------------------------------------------------------------
	// Get device information.
	//-----------------------------------------------------------------------
	uint8_t modelName[16];
	if (readReg(fd, deviceAddress, 0, modelName, 16) < 0) {
		printf("Failed to read device register\n");
		exit(1);
	}
	printf("Model: %s\n", modelName);

	uint32_t hardwareRevision;
	if (readReg(fd, deviceAddress, 1, (uint8_t*)&hardwareRevision, 4) < 0) {
		printf("Failed to read device register\n");
		exit(1);
	}
	printf("Hardware: %d\n", hardwareRevision);

	uint8_t firmwareVersion[4];
	if (readReg(fd, deviceAddress, 2, firmwareVersion, 4) < 0) {
		printf("Failed to read device register\n");
		exit(1);
	}
	printf("Firmware version: %u.%u.%u\n", firmwareVersion[2], firmwareVersion[1], firmwareVersion[0]);

	uint8_t serial[16];
	if (readReg(fd, deviceAddress, 3, serial, 16) < 0) {
		printf("Failed to read device register\n");
		exit(1);
	}
	printf("Serial: %s\n", serial);

	//-----------------------------------------------------------------------
	// Perform upgrade.
	//-----------------------------------------------------------------------
	if (performUpgrade(fd, deviceAddress, "./sf20_16_4.1.2.lwf") < 0) {
		printf("Failed to upgrade device\n");
		exit(1);
	}

	//-----------------------------------------------------------------------
	// Verify upgrade.
	//-----------------------------------------------------------------------
	// Wake device and engage i2c protocol.
	if (writeReg(fd, deviceAddress, 120, (uint8_t*)&cmd, 2) < 0) {
		// NOTE: This is written twice because the first time may only wake the i2c bus of the unit.
		sleep(1);

		if (writeReg(fd, deviceAddress, 120, (uint8_t*)&cmd, 2) < 0) {
			printf("Failed to write device register\n");
			exit(1);
		}
	}

	if (readReg(fd, deviceAddress, 2, firmwareVersion, 4) < 0) {
		printf("Failed to read device register\n");
		exit(1);
	}
	printf("New firmware version: %u.%u.%u\n", firmwareVersion[2], firmwareVersion[1], firmwareVersion[0]);

	return 0;
}
//-------------------------------------------------------------------------
// LightWare SF11 Linux Example
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

// Write a 16bit entry into the log.
void logData(uint16_t Data) {
	// NOTE: Needs to be implemented based on platform.
}

//-------------------------------------------------------------------------
// Main application.
//-------------------------------------------------------------------------
int main(int args, char** argv)
{
	printf("SF11 LWNX sample\n");

	// Make sure the SF11 has serial output baud rate configured to 921600.
	// 921600 bps is needed to support the incoming data which is ~60 KB/s.
	portConnect("/dev/ttyUSB0", B921600);

	// Setup serial port callbacks so the LWNX protocol can read/write data through the serial port.
	lwEndpoint endpoint = {};
	endpoint.writeCallback = portWrite;
	endpoint.readCallback = portRead;
	endpoint.timeCallback = getTimeMilliseconds;

	// Read the product name. (Command 0: Product name)
	// At least one command is needed to activate LWNX mode. That's why this is here.
	char modelName[16] = {};
	lwnxCmdReadString(&endpoint, 0, modelName);
	printf("Hardware name: %s\n", modelName);

	// Enable streaming of waveform data. (Command 30: Stream)
	lwnxCmdWriteUInt32(&endpoint, 30, 1);

	int32_t waveformSampleId = 0;

	// Continuously wait for and process incoming data packets.
	while (1) {
		lwResponsePacket response = {};

		// Wait for next data packet.
		if (lwnxRecvPacketAny(&endpoint, &response, 1000)) {
			
			if (response.cmdId == 32) {
				// Packet 32 contains waveform samples.

				uint8_t sampleCount = response.data[4];
				
				for (int i = 0; i < sampleCount; ++i) {
					int32_t idx = 6 + i * 4;
					uint16_t value = (uint16_t)response.data[idx + 0] | ((uint16_t)response.data[idx + 1] << 8);
					int8_t waveformStart = (value & 0x8000) != 0;
					uint16_t waveformSample = value & 0x7FFF;

					if (waveformStart) {
						waveformSampleId = 0;
						
						// Log a marker to indicate the begining of a waveform.
						logData(0xFFFF);
					} else {
						++waveformSampleId;

						// Only need to log every 3rd sample of the waveform data.
						// There are roughly 1450 samples per waveform.
						// Each sample is 2 bytes.
						// 1450 / 3 * 2 = ~0.9 KB per waveform.
						if (waveformSampleId % 3 == 0) {
							logData(waveformSample);
						}
					}
				}
			} else if (response.cmdId == 39) {
				// Packet 39 contains distance results (as would normally be output by the sensor).
				// You can use the filtered distance as if it was a normal reading retrieved by sending 'd'.

				float filteredDistance = 0;
				memcpy(&filteredDistance, &response.data[55], 4);

				printf("Distance info: %f\n", filteredDistance);
			}
		}
	}

	return 0;
}
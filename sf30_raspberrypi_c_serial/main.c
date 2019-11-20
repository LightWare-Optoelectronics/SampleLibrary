//-------------------------------------------------------------------------
// LightWare SF30 serial interface linux sample.
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdint.h>
#include "time.h"

//-------------------------------------------------------------------------
// Platform Implementation.
//-------------------------------------------------------------------------
// Get the time in milliseconds from the system. Does not need to start at 0.
int32_t getTimeMilliseconds() {
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);

	return (time.tv_sec * 1000000 + time.tv_nsec / 1000) / 1000;
}

// Initiate a serial port connection.
int portConnect(const char* Name, int BitRate) {
	int result = -1;
	printf("Attempt com connection: %s\n", Name);
		
	result = open(Name, O_RDWR | O_NOCTTY | O_SYNC);
	
	if (result < 0) {
		printf("Couldn't open serial port!\n");
		return -1;
	}

	struct termios tty;
	memset(&tty, 0, sizeof(tty));
	if (tcgetattr(result, &tty) != 0) {
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

	if (tcsetattr(result, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr\n");
		return -1;
	}

	printf("Connected\n");
	
	return result;
}

// Write BufferSize bytes to the serial port from Buffer.
int portWrite(int Serial, uint8_t* Buffer, int32_t BufferSize) {
	return write(Serial, Buffer, BufferSize);
}

// Write Buffer (a null terminating string) to the serial port.
int portWriteStr(int Serial, const char* Buffer) {
	int len = strlen(Buffer);

	return write(Serial, Buffer, len);
}

// Read up to BufferSize bytes from the serial port into Buffer.
int portRead(int Serial, uint8_t* Buffer, int32_t BufferSize) {
	return read(Serial, Buffer, BufferSize);
}

//-------------------------------------------------------------------------
// Main application.
//-------------------------------------------------------------------------
int main(int args, char** argv) {
	printf("SF30 serial sample\n");

	// Make sure the SF30 has serial output enabled and baud rate configured to 460800.
	// NOTE: I have not had luck running the PI serial port at 921600.
	int serial = portConnect("/dev/ttyS0", B460800);
	
	if (serial != -1) {
		// Set exposure time to 156 readings per second.
		portWriteStr(serial, "#R7:");

		// Set serial output rate to 156 readings per second.
		portWriteStr(serial, "#U7:");

		// Set the return mode to last return.
		portWriteStr(serial, "#S1:");

		// Set the zero offset to 0 m.
		portWriteStr(serial, "#Z0:");

		int byteState = 0;
		int byteH = 0;

		while (true) {
			// NOTE: This delay is just a temporary measure to prevent the loop from fully occupying the CPU.
			// Ideally the rest of your program code would happen here. If the program requires waiting on the 
			// serial port then you can use some form of select/poll that will not block the CPU.
			usleep(1000);

			uint8_t buffer[1024];
			uint8_t r = portRead(serial, buffer, 1024);

			if (r == -1) {
				printf("Error reading serial port\n");
				break;
			}

			// Parse the distance bytes.
			for (int i = 0; i < r; ++i) {
				if (buffer[i] & 0x80) {
					byteState = 1;
					byteH = buffer[i] & 0x7F;
				} else {
					if (byteState) {
						byteState = 0;
						int distance = (byteH << 7) | buffer[i];

						if (distance == 16000) {
							printf("Lost signal\n");
						} else {
							printf("%d cm\n", distance);
						}
					}
				}
			}
		}
	}

	printf("Program terminated\n");

	return 0;
}
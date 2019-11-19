//-------------------------------------------------------------------------------------------
// LightWare SF11 serial connection sample
// https://lightware.co.za
//-------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

//-------------------------------------------------------------------------
// Com Port Implementation.
//-------------------------------------------------------------------------
int portConnect(const char* Name, int BitRate) {
	int fd = -1;
	printf("Attempt com connection: %s\n", Name);
		
	fd = open(Name, O_RDWR | O_NOCTTY | O_SYNC);
	
	if (fd < 0) {
		printf("Couldn't open serial port!\n");
		return -1;
	}

	struct termios tty;
	memset(&tty, 0, sizeof(tty));
	if (tcgetattr(fd, &tty) != 0) {
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

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr\n");
		return -1;
	}

	printf("Connected\n");
	
	return fd;
}

int portWrite(int SerialPortFd, const char* Buffer, int BufferSize) {
	if (SerialPortFd < 0) {
		printf("Can't write to null coms\n");
		return -1;
	}

	int writtenBytes = write(SerialPortFd, Buffer, BufferSize);

	if (writtenBytes != BufferSize)	{
		printf("Could not send all bytes! (%d)\n", writtenBytes);
		return writtenBytes;
	}

	return writtenBytes;
}

int portRead(int SerialPortFd, char* Buffer, int BufferSize) {
	if (SerialPortFd < 0) {
		printf("Can't read from null coms\n");
		return -1;
	}

	int readBytes = read(SerialPortFd, Buffer, BufferSize);

	return readBytes;
}

//-------------------------------------------------------------------------
// USB port sample.
//-------------------------------------------------------------------------
// The USB port interface for the SF11 was designed to allow humans to
// easily interact and configure settings. It does not output in the same
// way as the serial port interface on the SF11. Make sure that you have
// not set the SF11 into menu mode when trying to read from this port.
//-------------------------------------------------------------------------
void runUsbPortSample() {
	printf("Connecting on USB interface...\n");

	int serialPortFd = portConnect("/dev/ttyUSB0", B115200);

	if (serialPortFd < 0)
		return;

	// Current packet.
	char packetBuffer[64];
	int packetSize = 0;	

	while (1) {
		// Wait for 50ms (50 000 us).
		usleep(50000);
		
		// Get bytes from the port.
		char buffer[64];
		int bytesRead = portRead(serialPortFd, buffer, sizeof buffer);

		// Look for distance packets, each is separated by a newline.
		for (int i = 0; i < bytesRead; ++i) {			
			char c = buffer[i];

			if (c == '\n') {
				// End of line found, so process the collected data.
				packetBuffer[packetSize] = 0;
				// printf("Packet: [%s]\n", packetBuffer);

				float distance;
				float analogVoltage;
				float strength;

				int r = sscanf(packetBuffer, "%f m %f V %f", &distance, &analogVoltage, &strength);

				if (r == 3) {
					// If we received a full packet then output the distance.
					printf("Distance: %0.2f m\n", distance);
				}

				// Reset the current packet.
				packetSize = 0;

			} else if (c == '\r') {
				// Ignore carriage returns.
			} else {
				if (packetSize < (sizeof packetBuffer) - 1) {
					packetBuffer[packetSize++] = c;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------
// Serial port sample.
//-------------------------------------------------------------------------
// The serial port interface for the SF11 was designed to allow machines to
// easily interact and configure settings. It does not output in the same
// way as the USB interface on the SF11.
//-------------------------------------------------------------------------
void runSerialPortSample() {
	printf("Connecting on USB interface...\n");

	int serialPortFd = portConnect("/dev/ttyUSB0", B115200);

	if (serialPortFd < 0)
		return;

	// Current packet.
	char packetBuffer[64];
	int packetSize = 0;	

	while (1) {
		// The distance needs to be requested by sending the 'd' character.
		const char* distanceRequest = "d";
		portWrite(serialPortFd, distanceRequest, 1);

		// Wait for 50ms (50 000 us).
		usleep(50000);
		
		// Get bytes from the port.
		char buffer[64];
		int bytesRead = portRead(serialPortFd, buffer, sizeof buffer);

		// Look for distance packets, each is separated by a newline.
		for (int i = 0; i < bytesRead; ++i) {			
			char c = buffer[i];

			if (c == '\n') {
				// End of line found, so process the collected data.
				packetBuffer[packetSize] = 0;
				
				// If we received a full packet then output the distance.
				float distance = atof(packetBuffer);
				printf("Distance: %0.2f m\n", distance);

				// Reset the current packet.
				packetSize = 0;

			} else if (c == '\r') {
				// Ignore carriage returns.
			} else {
				if (packetSize < (sizeof packetBuffer) - 1) {
					packetBuffer[packetSize++] = c;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------
// Application Entry.
//-------------------------------------------------------------------------
int main(int args, char** argv)
{
	printf("SF11 sample\n");

	runUsbPortSample();
	//runSerialPortSample();
	
	return 0;
}
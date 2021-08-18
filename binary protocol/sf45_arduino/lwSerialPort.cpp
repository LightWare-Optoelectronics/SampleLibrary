#include "lwSerialPort.h"

bool lwSerialPort::connect(Stream* Port, int BitRate) {
	_port = Port;
	return true;
}

bool lwSerialPort::disconnect() {
	return true;
}

int lwSerialPort::writeData(uint8_t *Buffer, int32_t BufferSize) {
	_port->write(Buffer, BufferSize);
	return BufferSize;
}

int32_t lwSerialPort::readData(uint8_t *Buffer, int32_t BufferSize) {
	int bytesRead = 0;
	while (true) {
		int c = _port->read();

		if (c == -1) {
			return bytesRead;
		} else {
			Buffer[bytesRead++] = c;
			if (bytesRead == BufferSize) {
				return bytesRead;
			}
		}
	}

	return 0;
}
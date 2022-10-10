#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

static uint8_t _tempBuffer[1024];

int i2cOpenDevice(const char* device) {
	int fd = open(device, O_RDWR);
	return fd;
}

int writeReg(int fd, uint8_t address, uint8_t reg, uint8_t* buffer, uint8_t length) {
	memcpy(_tempBuffer + 1, buffer, length);
	_tempBuffer[0] = reg;

	struct i2c_msg messages[] = {
    	{ address, 0, length + 1, _tempBuffer },
	};

  	struct i2c_rdwr_ioctl_data ioctl_data = { messages, 1 };
  	int result = ioctl(fd, I2C_RDWR, &ioctl_data);

  	if (result != 1)  {
    	return -1;
  	}

  	return 0;
}

int readReg(int fd, uint8_t address, uint8_t reg, uint8_t* buffer, uint8_t length) {
	uint8_t command[] = { reg };

	struct i2c_msg messages[] = {
		{ address, 0, sizeof(command), command },
		{ address, I2C_M_RD, length, buffer },
	};

	struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };
	int result = ioctl(fd, I2C_RDWR, &ioctl_data);

	if (result != 2)  {
		return -1;
	}

	return 0;
}
#include "platform.h"
#include "lwSerialPort.h"

void platformInit() {

}

double getTimeSeconds() {

	return 0;
}

int64_t platformGetMicrosecond() {
	return (int64_t)(micros());
}

int32_t platformGetMillisecond() {
	return (int32_t)(millis());
}

bool platformSleep(int32_t TimeMS) {
	delay(TimeMS);
	return true;
};
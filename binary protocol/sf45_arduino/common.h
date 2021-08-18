#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <Stream.h>
#include <Arduino.h>

#include "lwSerialPort.h"

#include "platform.h"

//----------------------------------------------------------------------------------------------------------------------------------
// Helper utilities.
//----------------------------------------------------------------------------------------------------------------------------------
void printHexDebug(uint8_t* Data, uint32_t Size);
//-------------------------------------------------------------------------
// This file implements platform specific functions while following an
// interface that the core code will be calling.
//-------------------------------------------------------------------------
#pragma once

#include "common.h"

void platformInit();

int64_t platformGetMicrosecond();
int32_t platformGetMillisecond();
bool platformSleep(int32_t TimeMS);

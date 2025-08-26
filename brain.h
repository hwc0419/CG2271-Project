#ifndef BRAIN_H
#define BRAIN_H

#include <stdint.h>
#include "uart.h"

typedef struct {
	speedL leftMotor;
	speedR rightMotor;
	dirL leftSpinDir;
	dirR rightSpinDir;
	uint8_t isReady;
	uint8_t isDone;
} msg;

void brainThread(void*);
#endif

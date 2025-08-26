#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include "motor.h"

#define BAUD_RATE 9600

void initUARTRX(uint32_t baudrate);
#endif

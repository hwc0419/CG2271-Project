#ifndef LED_H
#define LED_H

#include "MKL25Z4.h"                    // Device header
#include <stdint.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

typedef uint32_t enablePins;
typedef uint8_t muxOption;
typedef uint32_t blinkRate;
typedef uint32_t cycleRate;

typedef enum{
	LED_0, //PTE5
	LED_1, //PTE4
	LED_2, //PTE3
	LED_3, //PTE2
	LED_4, //PTB11
	LED_5, //PTB10
	LED_6, //PTB9
	LED_7, //PTB8
} ledNo;

typedef struct {
	enablePins pins[3];
	blinkRate bRate;
	cycleRate cRate;
	ledNo num;
} ledStatus;

#define LED_TOTAL 8
//lsb = pin0, msb = pin31
#define LED_PINS_C (enablePins)(0x00000CF9) 
#define LED_PINS_B (enablePins)(0x00000F00)
#define LED_PINS_E (enablePins)(0x0000003C)

#define PTE5 5
#define PTE4 4
#define PTE3 3
#define PTE2 2
#define PTB11 11
#define PTB10 10
#define PTB9 9
#define PTB8 8

#define C 0
#define B 1
#define E 2

//blink rate in ms
#define BLINK_FAST 250
#define BLINK_SLOW 500

//cycle rate in ms
#define CYCLE_RATE 250

#define MASK(x) (uint32_t)(1 << (x))

//all led pins are configured to GPIO
static muxOption options[32] = {1};

void initLED(void);
void blinkRedLED(enablePins);
void cycleGreenLED(enablePins, enablePins, ledNo);
void onGreenLED(enablePins, enablePins);
void ledThread(void *);
#endif

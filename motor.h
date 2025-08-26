#ifndef MOTOR_H
#define MOTOR_H

#include "MKL25Z4.h"                    // Device header
#include <stdint.h>
#include "pinout.h"

//duty cycles
#define CYCLE100 100
#define CYCLE80 80
#define CYCLE60 45
#define CYCLE0 0

#define N 128 							// prescalar
#define FREQ 50 						// desired frequency
#define MOD(N,FREQ) (uint32_t)(DEFAULT_SYSTEM_CLOCK / (N * FREQ)) // cannot exceed 2^16-1 = 65535
#define CNV(DUTYCYCLE,MOD) (uint32_t)(MOD * DUTYCYCLE / 100) // cannot exceed 2^16-1 = 65535

typedef enum
{
	MAXL,
	HIGHL,
	MEDL,
	ZEROL
} speedL;

typedef enum
{
	MAXR,
	HIGHR,
	MEDR,
	ZEROR
} speedR;


typedef enum
{
	CW_L, //FRONT
	ACW_L //BACK
} dirL;

typedef enum
{
	CW_R, //FRONT
	ACW_R //BACK
} dirR;

void setLeftPWM(uint8_t, uint8_t);
void setRightPWM(uint8_t, uint8_t);

void initPWM(void);
void move(speedL,speedR,dirL,dirR);
void moveThread(void*);
#endif

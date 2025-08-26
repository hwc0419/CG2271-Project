#include "uart.h"
#include "brain.h"
#include "motor.h"
#include "led.h"
#include "buzzer.h"

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

static msg command = { .leftMotor = ZEROL, 
											 .rightMotor = ZEROR, 
											 .leftSpinDir = CW_L, 
											 .rightSpinDir = CW_R, 
											 .isReady = 0, 
											 .isDone = 0};

static ledStatus ledStat = { .pins = {LED_PINS_C, LED_PINS_B, LED_PINS_E} , .bRate = BLINK_FAST, .cRate = 0, .num = LED_0};
static uint8_t dataSend, dataReceive;
osMutexId_t myMutex;
osEventFlagsId_t myEvent;
osMessageQueueId_t myQueue;

volatile uint8_t startRun = 0;
volatile uint8_t isFinished = 0; 

void UART2_IRQHandler(){
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if (UART2->S1 & UART_S1_RDRF_MASK){
		dataSend = UART2->D;
		osMessageQueuePut(myQueue, &dataSend, 0U, 0U);
		osEventFlagsClear(myEvent, 0x00000001);
	}
	PORTE->ISFR = 0xffffffff;
}

void brainThread(void *argument){
	for (;;) {
		osMessageQueueGet(myQueue, &dataReceive, 0U, 0U);
		command.leftMotor = (dataReceive >> 6) & 0x03;  // Extract bits [7:6]
		command.rightMotor = (dataReceive >> 4) & 0x03;  // Extract bits [5:4]
		command.leftSpinDir = (dataReceive >> 3) & 0x01;  // Extract bit [3]
		command.rightSpinDir = (dataReceive >> 2) & 0x01;  // Extract bit [2]
		command.isReady = (dataReceive >> 1) & 0x01;
		command.isDone = (dataReceive >> 0) & 0x01;
		osEventFlagsSet(myEvent, 0x00000001);
		if (command.isReady) {
			startRun = 1;
			isFinished = 0;
		} 
		if (command.isDone) {
			startRun = 1;
			isFinished = 1;
		} else {
			startRun = 0;
			isFinished = 0;
		}
	}
}

void moveThread(void *argument) {
	for (;;) {
		osEventFlagsWait(myEvent, 0x00000001, osFlagsWaitAll, 0);
		osMutexAcquire(myMutex,0);
		switch (command.leftMotor) {
			case MAXL:
				setLeftPWM(CYCLE100, command.leftSpinDir);
				break;
			case HIGHL:
				setLeftPWM(CYCLE80, command.leftSpinDir);
				break;
			case MEDL:
				setLeftPWM(CYCLE60, command.leftSpinDir);
				break;
			case ZEROL:
				setLeftPWM(CYCLE0, command.leftSpinDir);
				break;
		}
		switch (command.rightMotor) {
			case MAXR:
				setRightPWM(CYCLE100, command.rightSpinDir);
				break;
			case HIGHR:
				setRightPWM(CYCLE80, command.rightSpinDir);
				break;
			case MEDR:
				setRightPWM(CYCLE60, command.rightSpinDir);
				break;
			case ZEROR:
				setRightPWM(CYCLE0, command.rightSpinDir);
				break;
		}
		osMutexRelease(myMutex);
	}
}

void ledThread(void *argument) {
	for (;;) {
		osEventFlagsWait(myEvent, 0x00000001, osFlagsWaitAll, 0);
		if (command.leftMotor == ZEROL && command.rightMotor == ZEROR) {
			onGreenLED(ledStat.pins[B], ledStat.pins[E]);
			blinkRedLED(ledStat.pins[C]);
			//readjust blink rate
			ledStat.bRate = BLINK_FAST;
		} else {
			cycleGreenLED(ledStat.pins[B], ledStat.pins[E], ledStat.num);
			//update ledNo
			ledStat.num += 1;
			ledStat.num %= LED_TOTAL;
			
			blinkRedLED(ledStat.pins[C]);
			//readjust blink rate
			ledStat.bRate = BLINK_SLOW;
		}
		osDelay(ledStat.bRate);
	}
}

void audio_thread(void *argument) {
	for (;;) {
		osEventFlagsWait(myEvent, 0x00000001, osFlagsWaitAll, 0);
		playAudio();
	}
}

int main(void) {
	SystemCoreClockUpdate();
	initPWM();
	initLED();
	initAudioPWM();
	initUARTRX(BAUD_RATE);
	
	osKernelInitialize();
	myMutex = osMutexNew(NULL);
	myEvent = osEventFlagsNew(NULL);
	myQueue = osMessageQueueNew(32,1,NULL);
	osThreadNew(brainThread, NULL, NULL);
	osThreadNew(moveThread, NULL, NULL);
	osThreadNew(ledThread, NULL, NULL);
	osThreadNew(audio_thread, NULL, NULL);
	osKernelStart();
	for (;;) {}
}

#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include "brain.h"
#include "cmsis_os2.h"
#include <stdbool.h>

#define A_note 85
#define B_note 76
#define C_note 72
#define D_note 64
#define E_note 57
#define F_note 107
#define G_note 96

extern volatile uint8_t isFinished;
extern volatile uint8_t startRun;

void initAudioPWM(void);
void playAudio();

#endif
#include "MKL25Z4.h"                  
#include <stdbool.h>
#include <stdint.h>
#include "buzzer.h"
#include "brain.h"
#include "cmsis_os2.h"

#define PTE0_Pin 20
#define PTE1_Pin 21
#define PTD_Pin 4

// PORTE
void initAudioPWM(void) {
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; 
    
    PORTD->PCR[PTD_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[PTD_Pin] |= PORT_PCR_MUX(4); 
    
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    //Turns off Sound initially
    TPM0->MOD = 0;
    TPM0_C4V =  0;
    
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); 
    
    TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));  
    TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM0->SC &= ~(TPM_SC_CPWMS_MASK); 
    
    TPM0_C4SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM0_C4SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

int notes_run[] = {
    E_note, D_note, C_note, D_note, E_note, E_note, E_note, 
    D_note, D_note, D_note, E_note, G_note, G_note,
    E_note, D_note, C_note, D_note, E_note, E_note, E_note,
    E_note, D_note, D_note, E_note, D_note, C_note
};

int delay_run[] = {
    500, 500, 500, 500, 500, 500, 500,
    500, 500, 500, 500, 500, 500,
    500, 500, 500, 500, 500, 500, 
    500, 500, 500, 500, 500, 500, 500
};


int notes_end[] = {C_note, C_note, G_note, G_note, A_note, A_note, G_note,
                               F_note, F_note, E_note, E_note, D_note, D_note, C_note,
                               G_note, G_note, F_note, F_note, E_note, E_note, D_note,
                               G_note, G_note, F_note, F_note, E_note, E_note, D_note};

int delay_end[] = {500, 500, 500, 500, 500, 500, 500,
                                500, 500, 500, 500, 500, 500, 500,
                                500, 500, 500, 500, 500, 500, 500,
                                500, 500, 500, 500, 500, 500, 500};

volatile int notePtr = 0;
volatile int current_track = 0;
volatile int audio_track = 0;
volatile bool started = 0;
volatile bool finished = 0;

int* notes[] = {notes_run, notes_end};
int* delays[] = {delay_run, delay_end};
uint32_t note_size[] = {sizeof(notes_run) / sizeof(notes_run[0]), sizeof(notes_end) / sizeof(notes_end[0])};

void playAudio() {
	int notePtr = 0;
	int current_track = 0;
	int audio_track = 0;
	
	for(;;){
		if(!startRun){
			TPM0->MOD = 0;
			TPM0_C4V = 0;
			continue;
		}
		audio_track = isFinished? 1 : 0;
		if (audio_track != current_track) {
			notePtr = 0;
			current_track = audio_track;
		}
		//TPM0->MOD = notes[audio_track][notePtr];
		TPM0_C4V = TPM0->MOD / 4;
		osDelay(delays[audio_track][notePtr]);
		
		notePtr = (notePtr == note_size[audio_track] - 1)? 0 : notePtr + 1;
	}
}

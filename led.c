#include "led.h"
							 
void initLED(void) {
	//turn on power for PORTB,C,E
	SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK 
								|SIM_SCGC5_PORTC_MASK
								|SIM_SCGC5_PORTE_MASK);
	
	PORTC->PCR[0] &= ~(PORT_PCR_MUX_MASK);
	PORTE->PCR[2] &= ~(PORT_PCR_MUX_MASK);
	PORTE->PCR[3] &= ~(PORT_PCR_MUX_MASK);
	PORTE->PCR[4] &= ~(PORT_PCR_MUX_MASK);
	PORTE->PCR[5] &= ~(PORT_PCR_MUX_MASK);
	PORTB->PCR[8] &= ~(PORT_PCR_MUX_MASK);
	PORTB->PCR[9] &= ~(PORT_PCR_MUX_MASK);
	PORTB->PCR[10] &= ~(PORT_PCR_MUX_MASK);
	PORTB->PCR[11] &= ~(PORT_PCR_MUX_MASK);

	PORTC->PCR[0] |= PORT_PCR_MUX(1);
	PORTE->PCR[2] |= PORT_PCR_MUX(1);
	PORTE->PCR[3] |= PORT_PCR_MUX(1);
	PORTE->PCR[4] |= PORT_PCR_MUX(1);
	PORTE->PCR[5] |= PORT_PCR_MUX(1);
	PORTB->PCR[8] |= PORT_PCR_MUX(1);
	PORTB->PCR[9] |= PORT_PCR_MUX(1);
	PORTB->PCR[10] |= PORT_PCR_MUX(1);
	PORTB->PCR[11] |= PORT_PCR_MUX(1);
	
	//configure all pins as output
	//red
	PTC->PDDR |= LED_PINS_C;
	//green
	PTB->PDDR |= LED_PINS_B;
	PTE->PDDR |= LED_PINS_E;
}

void blinkRedLED(enablePins cPins) {
	PTC->PTOR |= cPins;
}

void cycleGreenLED(enablePins bPins, enablePins ePins, ledNo cycleCount) {
	PTE->PCOR |= ePins;
	PTB->PCOR |= bPins;
	
	switch (cycleCount) {
		case LED_0:
			PTE->PSOR |= MASK(PTE5); 
			break;
		case LED_1:
			PTE->PSOR |= MASK(PTE4); 
			break;
		case LED_2:
			PTE->PSOR |= MASK(PTE3); 
			break;
		case LED_3:
			PTE->PSOR |= MASK(PTE2); 
			break;
		case LED_4:
			PTB->PSOR |= MASK(PTB11); 
			break;
		case LED_5:
			PTB->PSOR |= MASK(PTB10); 
			break;
		case LED_6:
			PTB->PSOR |= MASK(PTB9); 
			break;
		case LED_7:
			PTB->PSOR |= MASK(PTB8); 
			break;
	}
}

void onGreenLED(enablePins bPins, enablePins ePins) {
	PTB->PSOR |= bPins;
	PTE->PSOR |= ePins;
}

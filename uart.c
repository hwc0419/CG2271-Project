#include "MKL25Z4.h"
#include "uart.h"
#include <stdbool.h>
#include "motor.h"

void initUARTRX(uint32_t baudrate){
  uint32_t divisor, bus_clock;
  
  bus_clock = DEFAULT_SYSTEM_CLOCK / 2;
  divisor = bus_clock / (baudrate * 16);
	
	// enable clock to the different peripheral blocks
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	//clear the rx reg
	PORTE->PCR[22] &= ~(PORT_PCR_MUX_MASK);
	PORTE->PCR[23] &= ~(PORT_PCR_MUX_MASK);
	
	//set the rx reg
	PORTE->PCR[23] |= PORT_PCR_MUX(4);
	PORTE->PCR[22] |= PORT_PCR_MUX(4);
	
	//Dont allow the interrupt enable for Receiving
	UART2->C2 &= ~(UART_C2_RE_MASK | UART_C2_RE_MASK);
	
	//set up the module
	UART2->BDH = UART_BDH_SBR(divisor >> 8); 
  UART2->BDL = UART_BDL_SBR(divisor); 

	// clear these modules
  UART2->C1 = 0; // set to one wire receive and one wire transmit
  UART2->S2 = 0;
  UART2->C3 = 0; //ninth-bit setting
	
	//turn on receiving
	UART2->C2 |= UART_C2_RE_MASK;
	
	//set interrupt
	NVIC_SetPriority(UART2_IRQn, 1);
  NVIC_ClearPendingIRQ(UART2_IRQn);
  NVIC_EnableIRQ(UART2_IRQn);
	
	UART2->C2 |= (UART_C2_RIE_MASK)|(UART_C2_RE_MASK);
}

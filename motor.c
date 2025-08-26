#include "motor.h"
#include "uart.h"

void setLeftPWM(uint8_t dutyCycle, uint8_t spinDir) {
  TPM1_C0V = spinDir ? CNV(dutyCycle, MOD(N,FREQ)) : CNV(CYCLE0, MOD(N,FREQ));
  TPM1_C1V = spinDir ? CNV(CYCLE0, MOD(N,FREQ)) : CNV(dutyCycle, MOD(N,FREQ));
}

void setRightPWM(uint8_t dutyCycle, uint8_t spinDir) {
  TPM2_C0V = spinDir ? CNV(dutyCycle, MOD(N,FREQ)) : CNV(CYCLE0, MOD(N,FREQ));
  TPM2_C1V = spinDir ? CNV(CYCLE0, MOD(N,FREQ)) : CNV(dutyCycle, MOD(N,FREQ));
}

void initPWM(void) {
  //turn on ports B 
  SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK);
  
  //configure PINB0-3 to be TPM
  for (uint8_t i = 0; i < 4; i++) {
     PORT_PCR_REG(PORTB,i) &= ~(PORT_PCR_MUX_MASK) ;
     PORT_PCR_REG(PORTB,i) |= PORT_PCR_MUX(PTB0_TPM1_CH0);
  }
 
  //turn on TPM1 and TPM2
  SIM->SCGC6 |= (SIM_SCGC6_TPM1_MASK
                     |SIM_SCGC6_TPM2_MASK);
 
  //select internal clock for TPM1 and TPM2
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
  
  //set LPTPM counter, Prescalar for both TPM1 and TPM2
  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
  
  TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
  
  //MOD = SYS_CLK / (N*FREQ)
  TPM1->MOD = MOD(N,FREQ);
  TPM2->MOD = MOD(N,FREQ);
  
  //configure all channels to low-true pulse
  for (uint8_t i = 0; i < 2; i++) {
     TPM_CnSC_REG(TPM1,i) &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
     TPM_CnSC_REG(TPM2,i) |= (TPM_CnSC_ELSA(1) | TPM_CnSC_MSB(1));
  }
}
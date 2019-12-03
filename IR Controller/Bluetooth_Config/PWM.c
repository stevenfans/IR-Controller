// PWM LED RED  PF.1
#include "PWM.h"
#include "../tm4c123gh6pm.h"

void PWM_Init(uint16_t period, uint16_t duty){
	volatile unsigned long delay; 
	SYSCTL_RCGCPWM_R  |= 0x02;			// Activate PWM1
	SYSCTL_RCGCGPIO_R |= 0x20;		  // Activate portF
	delay = SYSCTL_RCGCGPIO_R;   
	GPIO_PORTF_AFSEL_R |= 0x04;			// Enable AltFunctions for PF2
	// Use PWM on PF2
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & ~0x00000F00) | 0x00000500;
	GPIO_PORTF_AMSEL_R &= ~0x04;		// disable analog functions
	GPIO_PORTF_DEN_R |= 0x04;				// enable pbF
	
	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;
	SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M;
	SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2;
	
	PWM1_3_CTL_R = 0; 							// counter load value
	PWM1_3_GENA_R = 0xC8;						
	PWM1_3_LOAD_R = period - 1;			// cycles needed to count down to 0 
	PWM1_3_CMPA_R = duty - 1;
	
	PWM1_3_CTL_R |= 0x00000001;			// start pwm1
	PWM1_ENABLE_R |= 0x40;					// enable pf2/mw1pwm6
}

void PWM_PF2_Duty(uint16_t duty){
	PWM1_3_CMPA_R = duty - 1; 
}
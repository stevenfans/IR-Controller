// IR communcation using port A 
// used to demodulate a signal 
// with a running clock of 50 Mhz
#include "SysTick.h"
#include "IR_Demod.h"
#include "tm4c123gh6pm.h"
#include "IR_Demod.h"
#include "UART.h"

#define SIGNAL       				(*((volatile unsigned long *)0x40004030))

void Init_PortA(void){ 
	unsigned long volatile delay; 
	SYSCTL_RCGC2_R |= 0x00000001; 
	delay = SYSCTL_RCGC2_R; 
	GPIO_PORTA_DATA_R |= 0x04; 				//use PA2
	GPIO_PORTA_AFSEL_R &= ~0x04; 		//disable all alt funciton
	GPIO_PORTA_DEN_R |= 0x04; 				// used this pins 
	GPIO_PORTA_DIR_R &= ~0x04; 				// input pins on PA5~0
	GPIO_PORTA_CR_R |= 0x04;          // allow changes to PD5-0      
  GPIO_PORTA_AMSEL_R &= ~0x04;        // 3) disable analog function
  GPIO_PORTA_PCTL_R &= ~0x04; ;   // 4) GPIO clear bit PCTL 
	
	GPIO_PORTA_IS_R &= ~0x04;
	GPIO_PORTA_IBE_R &= ~0x04;
	GPIO_PORTA_IEV_R &= ~0x04;
	GPIO_PORTA_ICR_R = 0x04;
	GPIO_PORTA_IM_R |= 0x04;
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF0F) | 0x000000E0;
	NVIC_EN0_R |= 0x00000001;
}

void Init_PortE(void){ 
	unsigned long volatile delay; 
	SYSCTL_RCGC2_R |= 0x00000010; 
	delay = SYSCTL_RCGC2_R; 
	GPIO_PORTE_DATA_R |= 0x04; 				//use PA2
	GPIO_PORTE_AFSEL_R &= ~0x04; 		//disable all alt funciton
	GPIO_PORTE_DEN_R |= 0x04; 				// used this pins 
	GPIO_PORTE_DIR_R &= ~0x04; 				// input pins on PA5~0
	GPIO_PORTE_CR_R |= 0x04;          // allow changes to PD5-0      
  GPIO_PORTE_AMSEL_R &= ~0x04;        // 3) disable analog function
  GPIO_PORTE_PCTL_R &= ~0x04; ;   // 4) GPIO clear bit PCTL 
	
	GPIO_PORTE_IS_R &= ~0x04;
	GPIO_PORTE_IBE_R &= ~0x04;
	GPIO_PORTE_IEV_R &= ~0x04;
	GPIO_PORTE_ICR_R = 0x04;
	GPIO_PORTE_IM_R |= 0x04;
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF0F) | 0x000000E0;
	NVIC_EN0_R |= 0x00000010;
}


//void GPIOPortA_Handler(void){

//	if (GPIO_PORTA_RIS_R & 0x04){ // sw1 is pressed
//		GPIO_PORTA_ICR_R = 0x04; // clear flag
//		if (running ==0x00){
//			GPIO_PORTF_DATA_R = 0x02; 
//			running = 0xFF; // got the first bit 
//		}
//	}
//}



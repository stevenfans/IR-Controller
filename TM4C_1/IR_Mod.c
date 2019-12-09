// IR communcation using port A
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "IR_Mod.h"
#include "UART.h"

#define SIGNAL       				(*((volatile unsigned long *)0x40004030))
	
void Init_PortA(void){ 
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) A clock
  GPIO_PORTA_CR_R |= 0x0C;          // 2) allow changes to PA3, PA2  0000.1100
  GPIO_PORTA_AMSEL_R &= ~0xFF;      // 3) disable analog function
  GPIO_PORTA_PCTL_R &= ~0xFF;  		  // 4) GPIO clear bit PCTL  
  GPIO_PORTA_DIR_R = 0x0C;        	// 5) PB3-2 output 
  GPIO_PORTA_AFSEL_R &= ~0xFF;      // 6) no alternate function
  GPIO_PORTA_DEN_R |= 0x0C;         // 7) enable digital pins PA3-2   
}

void modulateSignal(void){
	SIGNAL = 0xFF;
	SysTick_Wait19Khz(); 
	SIGNAL = 0x00; 
	SysTick_Wait19Khz(); 
}
void nomodulateSignal(void){
	SIGNAL = 0x00; 
	SysTick_Wait19Khz(); 
	SIGNAL = 0x00; 
	SysTick_Wait19Khz(); 
}
// 1ms high = 38 pulses,  800us low = 30 pulses
// 38/19
void startPulse(void){
	unsigned long i; 
	for(i=0; i<38; i++){
		modulateSignal(); 
	}
	for(i=0; i<35; i++){
		nomodulateSignal(); 
	}
}
// 900us high = 34 pulses, 450us low = 17 pulses
void logical_1(void){
	unsigned long i; 
	for(i=0; i<34; i++){
		modulateSignal(); 
	}
	for(i=0; i<17; i++){
		nomodulateSignal(); 
	}
}

// 450us high = 17 pulses, 450us low = 17 pulses
void logical_0(void){
	unsigned long i; 
	for(i=0; i<17; i++){
		modulateSignal(); 
	}
	for(i=0; i<17; i++){
		nomodulateSignal(); 
	}
}

void sendPackage(char*ptr, char*cmd){
	char bin; 
	unsigned long number; 
	int i; 
	
	startPulse(); 
//	logical_0(); 
//	logical_1(); 
	//UART_OutUDec(ptr[1]); UART_OutUDec(ptr[0]); 
	for(i=1;i>=0; i--){
		bin = ptr[i];
		//UART_OutUDec(number);
		if(bin==1) {
			logical_1(); 
			UART1_OutChar('1'); 
		}
		else {
			logical_0(); 
			UART1_OutChar('0'); 
		}
	}
	for(i=2; i>=0; i--){
		bin = cmd[i];
		if(bin==1) {
			logical_1(); 
			UART1_OutChar('1'); 
		}
		else {
			logical_0(); 
			UART1_OutChar('0'); 
		}
	}
}

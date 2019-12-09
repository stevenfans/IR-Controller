// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
#define SIGNAL       				(*((volatile unsigned long *)0x40004030))
#define LIGHT 							(*((volatile unsigned long *)0x40025038))
#define RED 		0x02; 
#define BLUE 		0x04; 
#define GREEN   0x08; 
#define WHITE   0x0E; 

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "UART.h"
#include "SysTick.h"
#include "PWM.h"
#include "IR_Mod.h"
#include <math.h>
#include <stdlib.h>


void PORTF_Init(void);
void EnableInterrupts(void);
unsigned long stringToNumber(char string[4]);
char *decimalToBin(char*arr, int decimal); 

unsigned long device_number = 0; //default 0

int main(void){
//	unsigned char x;
//  unsigned char i;
	unsigned long command_decimal_num, device_value; 
  char string[5]; 			// global to assist in debugging

	// variables for IR LED
	char address[2]; //holds binary value of the device number
	char command[3]; //holds binary value of the command number
	char *addr_ptr; 
	char *cmd_ptr; 

//	unsigned char n;																		
	unsigned char isFrequency = 0x00; // flag to check if there is an f 
	char a = 0xFF;
	int sum = 0;
	int n = 0; 
	
	SysTick_Init();  
	Init_PortA(); 
  UART_Init();              // initialize UART
  UART1_Init(); 						// initialize UART1
	UART2_Init(); 						// initialize UART2
	
	PORTF_Init(); 
	EnableInterrupts();          		 	//AFTER inits, 
	PLL_Init();
	
	LIGHT = RED; //start off with red
	//startPulse(); 
  while(1){
		n = 0;
		//startPulse(); 
		//SysTick_Wait(2000000); 
		//startPulse(); 
		//logical_0(); 
		//logical_1(); 
		//startPulse(); 
		UART1_OutString("Enter Something: "); UART_OutChar('s'); 
		UART1_InString(string, 1);  OutCRLF1(); 
		command_decimal_num = string[0]-'0';

	//turn the device number to a binary value
		//UART_OutUDec(device_number); 	
		addr_ptr = decimalToBin(address,device_number); 
	//turn the command value to binary value
		//UART_OutUDec(command_decimal_num);
		cmd_ptr = decimalToBin(command, command_decimal_num); 
		
		sendPackage(addr_ptr, cmd_ptr);
	}
}

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C	
// white    RGB    0x0E
// pink     R-B    0x06

// takes in a string of 3 elements and turn it to a a number
unsigned long stringToNumber(char string[3]){
	int i;  
	int answer = 0; 
	int place_value = 1; 
	for (i=3;i>0;i--){
		if (string[i]!=CR&&string[i]!=LF){//check if there is not a null value
		UART_OutChar(string[i]); 
		answer += place_value * (string[i]-'0'); // convert decimal to char   
		place_value *= 10; 
		}
	}
	//UART_OutUDec(answer); 
	return answer; 
}

// return a pointer to an array of binary values	
char *decimalToBin(char*arr, int decimal){
	unsigned int i = 0; 
	//UART_OutUDec(decimal); OutCRLF(); 
	if(decimal==0){//zerocase
		arr[0] = 0; 
		arr[1] = 0; 
		arr[2] = 0; 
	}
	//UART_OutChar('m');
	for(i=0;i<3;i++){
		arr[i] = decimal%2;  
		decimal = floor(decimal/2);
		UART_OutUDec(decimal); 
		UART_OutUDec(arr[i]);		
	}
	return arr;  
}

// global variable visible in Watch window of debugger
// increments at least once per button press
void PORTF_Init(void){      
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        	
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

void GPIOPortF_Handler(void){
	if ((GPIO_PORTF_RIS_R & 0x10)){ // sw1 is pressed
		GPIO_PORTF_ICR_R = 0x10;
		if (LIGHT==0x08) {
			LIGHT = WHITE;
			device_number = 3; 
		}
		else if (LIGHT==0x0E){
			LIGHT=RED;
			device_number = 0; 
		}
		else  {
			LIGHT = LIGHT << 1; 
			device_number +=1; 
		}
	}
}

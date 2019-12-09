#include "PLL.h"
#include "UART.h"
#include "Tone.h"
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "IR_Demod.h"

#define SIGNAL       				(*((volatile unsigned long *)0x40004030))
#define LIGHT 							(*((volatile unsigned long *)0x40025038))
#define RED 		0x02; 
#define BLUE 		0x04; 
#define GREEN   0x08; 
#define WHITE   0x0E; 

// states
#define WAIT_FOR_START 		0;
#define FIRST 	1; 
#define SECOND 	2; 
#define THIRD 	3; 
#define FOURTH 	4; 
#define FIFTH 	5; 
int state = 0 ; 

int running = 0; 
int startPulseDetected =0x00; 
unsigned int tally_1 = 0; // hold the count for every time there is a 1 
unsigned int tally_0 = 0; // hold the count for every time there is a 0
int  packet[5]; // array to hold 1 or 0
unsigned long packet_element = 0; 
unsigned long loop_0,loop_1=0; 
int testingFlag = 0; 
int firstTime = 0; 


void PORTF_Init(void);
void Timer0_Init(); 

void checkStartPulse(); 
void check1(); 
void check0(); 


//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}

int main(void){
	unsigned long frequency = 262;
	unsigned long last_frequency = 262;
	unsigned long freq_value;  
	char string [10]; 
	unsigned int k ; 
	
	PLL_Init();
  UART_Init();
	UART2_Init();
	
	Init_PortA(); 
	Init_PortB();
	PORTF_Init();
	SysTick_Init(800); 
	Timer0_Init();
	
	state = WAIT_FOR_START; 
	//GPIO_PORTF_DATA_R = 0x04; 
	UART_OutChar('a'); 
  while(1){
		//UART_OutUDec(state); 

		UART_OutUDec(running); UART_OutUDec(startPulseDetected);  OutCRLF(); 
		UART_OutString("Signal 0 Count: "); UART_OutUDec(tally_0);
		UART_OutString("    Signal 1 Count: "); UART_OutUDec(tally_1);
		OutCRLF(); 
	if(testingFlag ==1) {UART_OutString("TESTING HERE:");UART_OutUDec(packet_element); 
		OutCRLF(); }
//		UART_OutString("loop0: "); UART_OutUDec(loop_0);
//		UART_OutString("    loop1: "); UART_OutUDec(loop_1);
//		OutCRLF(); 
		UART_OutString("Packet Array: ");
		for(k=0; k<5;k++){
			UART_OutUDec(packet[k]);
		}
		OutCRLF(); 
		
		switch(state){
			case 0: if (running==1) checkStartPulse(); UART_OutString("In state 0"); OutCRLF(); 
							break; 
			case 1: UART_OutString("In state2"); 
							break; 
			case 2: UART_OutString("State 3"); 
							break ;
			case 3: UART_OutString("State 4"); 
							break ;
			default: UART_OutString("Default State"); 
							break; 
		}
		SysTick_Wait1us(999999);
  }
}

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


// interrupt checks to see the start of the decoding
void GPIOPortA_Handler(void){
	if (GPIO_PORTA_RIS_R & 0x04){ // sw1 is pressed
		GPIO_PORTA_ICR_R = 0x04; // clear flag
		GPIO_PORTF_DATA_R = 0x08; 
		if (running==0&&firstTime==0){
			running = 1; 
			firstTime=1; 
		}
	}
}

void checkStartPulse(){
	//determine if its there is an accurate start pulse
	if(90<tally_0 && tally_0<120 &&
		  70<tally_1 && tally_1<120){
				tally_0 = 0; 
				tally_1 = 0; 
				UART_OutString("FUCK ME"); 
				state = FIRST;
				running = 0; 
				startPulseDetected = 1; 
			}
}

void check1(){
	if(90<tally_0 && tally_0<100 &&
		  40<tally_1 && tally_1<60){
				running=0; 
				startPulseDetected=1; 
				tally_0 = 0; 
				tally_1 = 0; 
				packet[packet_element]=1; 
				state = SECOND;
				packet_element++;
				testingFlag=1; 
			}
}

void check0(){
	if(35<tally_0 && tally_0<60 &&
		 35<tally_1 && tally_1<100){
				running=0; 
				//startPulseDetected=1; 
				tally_0 = 0; 
				tally_1 = 0; 
				packet[packet_element]=9;
				state = THIRD;
				packet_element++;
				testingFlag=1; 
			}
}

void SysTick_Handler(){
}

void Timer0_Init(){//10 us
	SYSCTL_RCGCTIMER_R |= 0x01; //Enable Timer0
	TIMER0_CTL_R   = 0; //disable timer0a for setup
	TIMER0_CFG_R   = 0x00000000; //32 bit mode
	TIMER0_TAMR_R  = 0x00000002; //periodic mode, down-count
	TIMER0_TAILR_R = 800;//10 us period
	TIMER0_TAPR_R  = 0;//bus clock resolution?
	TIMER0_ICR_R   = 0x00000001;//clear TIMER0A flag
	TIMER0_IMR_R   = 0x00000001;//arm timer interrupt
	NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000; //priority 1
	NVIC_EN0_R = 1<<19;
	
	TIMER0_CTL_R = 0x00000001;    //enable TIMER0A
}

void Timer0A_Handler(){//called every 100 us
	 TIMER0_ICR_R = 0x00000001; //acknowledge timer0A flag
	
	 //GPIO_PORTF_DATA_R ^= RED;
	if(packet_element>=5) startPulseDetected=0; //packet_element=0; 
	if(tally_1>=150)tally_1=0; 
	
	
	if (running){
		if (SIGNAL==0) tally_0++; 
		else tally_1++; 
	}
	
	
	checkStartPulse(); 

	if (startPulseDetected){
		
		check0(); check1(); 
		if (SIGNAL==0) tally_0++; 
		else tally_1++; 
	}
	 check0();check1(); 
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//	if(150<tally_0 || 150<tally_1){
//		tally_0 = 0; 
//		tally_1 = 0; 
//		running = 0; 
//	}
//	
//	if(running==1){ //recieved a modulated signal 
//		
//		//tally the counter to determine if its a start/logic 1/logic 0
//		GPIO_PORTF_DATA_R ^= 0x08;
//		if (SIGNAL==0) tally_0++; 
//		else tally_1++; 
//		//-------------------------------------------------------
//		if (startPulseDetected==1){ // when there is a legit signal
//			//update the counters
//			if (SIGNAL==0) tally_0++; 
//			else tally_1++; 
//			
//			//check if its a 0
//			if(40<tally_0 && tally_0<70 && 
//				 40<tally_1 && tally_1<70){
//					 // add 0 to the array
//					 packet[packet_element] = 0; 
//					 tally_0 = 0; 
//					 tally_1 = 0; 
//					 testingFlag = 1; 
//					 //running  = 0;
//					 loop_0++; 
//					 packet_element++;
//				 }
//		//check if its a 1
////		if(85<tally_0 && tally_0<100 &&
////			 40<tally_1 && tally_1<100){
////				 //do something here
////				 packet[packet_element] = 1; 
////				 tally_0 = 0; 
////				 tally_1 = 1; 
////				 //running  = 0;
////				 loop_1++; 
////				 packet_element++; 
////			 }
//		//packet_element++; //update the element index
//		 }
//	}
////	}
//	
//	//determine if its there is an accurate start pulse
//	if(90<tally_0 && tally_0<120 &&
//		  80<tally_1 && tally_1<500 &&
//		 running){	
//				startPulseDetected = 1; 
//				tally_0 = 0; 
//				tally_1 = 0; 
//				//running = 0; //reset the signal flag
//			}
}


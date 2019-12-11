#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "SysTick.h"
#include "PLL.h"
#include "waveforms.h"
#include "walking.h"
#include "tm4c123gh6pm.h"
#include "IR_Demod.h"
#include "UART.h"
#include "math.h"
\

#define SIGNAL       				(*((volatile unsigned long *)0x40024030))
#define LIGHT 							(*((volatile unsigned long *)0x40025038))
#define RED 		0x02; 
#define BLUE 		0x04; 
#define GREEN   0x08; 
#define WHITE   0x0E; 

int frequency = 262;
int running = 0; 
int startPulseDetected =0; 
int  packet[5] = {0,0,-1,-1,-1}; // array to hold 1 or 0
int new_packet = 0;
unsigned long packet_element = 0; 
unsigned long loop_0,loop_1=0; 
int testingFlag = 0; 
int firstTime = 0; 
int packetArray[1050]; 
int arrayFull = 0; 
int packet_array_element; 
int done=0; 
int current_device_number = 0; //default device 0
int previous_current_device_number = 0; //default device 0
int status, last_status = 0;
int input_device_number = 0;
int isSameDevice=0; 
int is_setup;

void PORTF_Init(void);
void Timer0_Init(void); 

int checkStartPulse(void); 
int isSameDeviceNum(void);
void decodePacket(void); 
int addressBinToDecimal(int *packet); 

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}

int main(void){
	unsigned int k ;
//	int input_device_number = 0;
	int previous_input_device_number = 0;	
	
	PLL_Init(4);
	SysTick_Init(80000000/256/frequency); // Clk / sine wave steps / desired frequency
	Init_PortB();
	UART_Init();
	UART2_Init();
	//Init_PortA(); 
	Init_PortE(); 
	PORTF_Init();
  ST7735_InitR(INITR_REDTAB);
	Timer0_Init();
	
	UART_OutChar('a'); 
	OutCRLF(); 
	LIGHT = RED; // default device 1
	
  while(1){		
		/*
		if(done){
		input_device_number = addressBinToDecimal(packet); 
		UART_OutString("Address to decimal: "); UART_OutUDec(input_device_number); OutCRLF(); 
			for(k=0; k<5;k++){
				UART_OutUDec(packet[k]);
			}
			OutCRLF(); 
			done=0; 
		}
		*/
	
		// Capture packet
		input_device_number = addressBinToDecimal(packet); 
		
		/*
		if (current_device_number == 0) previous_current_device_number = 0;
		else if (current_device_number == 1) previous_current_device_number = 1;
		else if (current_device_number  == 2) previous_current_device_number  = 2;
		else if (current_device_number  == 3) previous_current_device_number  = 3;
		//else current_device_number  = previous_current_device_number ;
		*/
		// Color    LED(s) PortF
		// dark     ---    0
		// red      R--    0x02
		// blue     --B    0x04
		// green    -G-    0x08
		// yellow   RG-    0x0A
		// sky blue -GB    0x0C	
		// white    RGB    0x0E
		// pink     R-B    0x06
		
		//UART_OutUDec(input_device_number);
		//UART_OutUDec(current_device_number);
		//OutCRLF();
		
		if(input_device_number==current_device_number){
			if (packet[2]== 0 && packet[3]==0 && packet[4] == 0){
				walking_animation(is_setup);
				is_setup = 1;
				previous_current_device_number  = 0;
				//GPIO_PORTF_DATA_R = 0x0A;
			}
			else if (packet[2]== 0 && packet[3]==0 && packet[4] == 1){
				walking_backwards_animation(is_setup);
				is_setup = 1;
				previous_current_device_number  = 1;
				//GPIO_PORTF_DATA_R = 0x0E; //white
				}
			else if (packet[2]== 0 && packet[3]==1 && packet[4] == 0){
				running_animation(is_setup);
				is_setup = 1;
				previous_current_device_number  = 2;
				//GPIO_PORTF_DATA_R = 0x06; //pink
			}
			else if (packet[2]== 0 && packet[3]==1 && packet[4] == 1){
				//GPIO_PORTF_DATA_R = 0x0C; // skyblue
				previous_current_device_number  = 3;
				is_setup = 1;
				running_backwards_animation(is_setup);
			}
			else{
				//GPIO_PORTF_DATA_R = 0x00;
				clear_animation();
			}
		}
		else{
			if (previous_current_device_number  == 0) {walking_animation(is_setup); is_setup = 1;}
			if (previous_current_device_number  == 1) {walking_backwards_animation(is_setup); is_setup = 1;}
			if (previous_current_device_number  == 2) {running_animation(is_setup); is_setup = 1;}
			if (previous_current_device_number  == 3) {running_backwards_animation(is_setup); is_setup = 1;}
		}
	}
}

//used to decode the address in binary to become an int
int addressBinToDecimal(int *packet){
	int place_value; 
	int answer=0; 
	int exponent=0; 
  
  for(place_value=1; place_value>=0; place_value--){
		if(packet[place_value]==1){
			answer += (packet[place_value]*pow(2,exponent)); 
		}
		else { 
			answer+= (packet[place_value]*pow(2,exponent)); 
    }
		exponent++;
  }
	return answer; 
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

void GPIOPortF_Handler(void){
	if ((GPIO_PORTF_RIS_R & 0x10)){ // sw1 is pressed
		GPIO_PORTF_ICR_R = 0x10;
		if (LIGHT==0x08) {
			LIGHT = WHITE;
			current_device_number = 3; 
		}
		else if (LIGHT==0x0E){
			LIGHT=RED;
			current_device_number = 0; 
		}
		else  {
			LIGHT = LIGHT << 1; 
			current_device_number +=1; 
		}
	}
}


// interrupt checks to see the start of the decoding
void GPIOPortE_Handler(void){
	if (GPIO_PORTE_RIS_R & 0x04){ // sw1 is pressed
		GPIO_PORTE_ICR_R = 0x04; // clear flag
		//GPIO_PORTF_DATA_R = 0x08; 
		if (running==0&&firstTime==0){
			running = 1; 
			firstTime=1; 
			packet_element=0;
			is_setup = 0;
		}
	}
	
}

int isSameDeviceNum(void){
	input_device_number = addressBinToDecimal(packet); 
	if (current_device_number!=input_device_number){
		return 0; 
	}
	else return 1; 
}

int checkStartPulse(){
	//determine if its there is an accurate start pulse
	if(packetArray[100] ==0){ // check to see if the signal was low 
		UART_OutString("Start Pulse Detected"); OutCRLF(); 
		return 1; 							// start signal should be low at this point 
	}
	else {
		UART_OutString("Start Pulse NOT Detected");
			 startPulseDetected=0; 
	 done = 1;
	 firstTime=0; 
		return 0; 
	}
 }

 void decodePacket(){
	 int count=267; 
	 unsigned int k; 
	 //skip the start pulse 
	 for(k=0;k<5;k++){
		 if(packetArray[count]==0){//logic 1
			 packet[k]=1;
			 count += 135;
			 //UART_OutString("*********   1    ********");
		 }
		 else{//logic 0
			 packet[k]=0; 
			 count+=88;
			 //UART_OutString("***********  0    **********");

		 }
	 }
	 startPulseDetected=0; 
	 done = 1;
	 firstTime=0; 
	 UART_OutString("DONE FLAG: "); UART_OutUDec(done); OutCRLF(); 
	 new_packet = 1;
//	 	for(k=0; k<1050;k++){
//				UART_OutUDec(packetArray[k]);
//			}
//			OutCRLF(); 
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

	if(running){
		if (SIGNAL==0) packetArray[packet_array_element]=0;  
		else packetArray[packet_array_element]=1;  	
		packet_array_element++;
	}
	if(packet_array_element==1050){
		running = 0;
		packet_array_element=0; 
		testingFlag = 1; 
		arrayFull=1; 
		startPulseDetected = checkStartPulse(); 
		isSameDevice = isSameDeviceNum(); 
	}
	
	if(startPulseDetected){// && isSameDeviceNum()){
		decodePacket(); 
	}
}

/*
unsigned long frequency_count = 0;
extern int packet[5];
void SysTick_Handler(){
	frequency_count++;
	
	if(input_device_number==current_device_number){
			if (packet[2]== 1 && packet[3]==0 && packet[4] == 0){
				sineWave(frequency_count);
				previous_current_device_number  = 4;
				//GPIO_PORTF_DATA_R = 0x0A;
			}
			else if (packet[2]== 1 && packet[3]==0 && packet[4] == 1){
				squareWave(frequency_count);
				previous_current_device_number  = 5;
				//GPIO_PORTF_DATA_R = 0x0E; //white
				}
			else if (packet[2]== 1 && packet[3]==1 && packet[4] == 0){
				triangleWave(frequency_count);
				previous_current_device_number  = 6;
				//GPIO_PORTF_DATA_R = 0x06; //pink
			}
			else if (packet[2]== 1 && packet[3]==1 && packet[4] == 1){
				//GPIO_PORTF_DATA_R = 0x0C; // skyblue
				previous_current_device_number  = 7;
				sawtoothWave(frequency_count);
			}
			else{
				//GPIO_PORTF_DATA_R = 0x00;
				clear_animation();
			}
		}
	else{
		if (previous_current_device_number  == 4) sineWave(frequency_count);
		if (previous_current_device_number  == 5) squareWave(frequency_count);
		if (previous_current_device_number  == 6) triangleWave(frequency_count);
		if (previous_current_device_number  == 7) sawtoothWave(frequency_count);
	}
	
	if (frequency_count == 255) frequency_count = 0;
}
*/
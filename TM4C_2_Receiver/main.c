// This is your first program to run on the LaunchPad
// You will run this program without modification as your Lab 2
// If the left switch SW1 is 
//      not pressed the LED toggles blue-red
//      pressed the LED toggles blue-green

// 0.Documentation Section 
// main.c
// Runs on LM4F120 or TM4C123
// Lab2_HelloLaunchPad, Input from PF4, output to PF3,PF2,PF1 (LED)
// Authors: Daniel Valvano, Jonathan Valvano and Ramesh Yerraballi
// Date: January 15, 2016

// LaunchPad built-in hardware
// SW1 left switch is negative logic PF4 on the Launchpad
// SW2 right switch is negative logic PF0 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad

// 1. Pre-processor Directives Section
// Constant declarations to access port registers using 
// symbolic names instead of addresses

#include "tm4c123gh6pm.h"

///////////////////////////////////////
//  Register definitions
///////////////////////////////////////
//Port f
//#define gpio_portf_data_r       (*((volatile unsigned long *)0x400253fc))
//#define gpio_portf_dir_r        (*((volatile unsigned long *)0x40025400))
//#define gpio_portf_afsel_r      (*((volatile unsigned long *)0x40025420))
//#define gpio_portf_pur_r        (*((volatile unsigned long *)0x40025510))
//#define gpio_portf_den_r        (*((volatile unsigned long *)0x4002551c))
//#define gpio_portf_lock_r       (*((volatile unsigned long *)0x40025520))
//#define gpio_portf_cr_r         (*((volatile unsigned long *)0x40025524))
//#define gpio_portf_amsel_r      (*((volatile unsigned long *)0x40025528))
//#define gpio_portf_pctl_r       (*((volatile unsigned long *)0x4002552c))
//#define sysctl_rcgc2_r          (*((volatile unsigned long *)0x400fe108))
//interrupt port f
//#define nvic_en0_r              (*((volatile unsigned long *)0xe000e100))  // irq 0 to 31 set enable register
//#define nvic_pri7_r             (*((volatile unsigned long *)0xe000e41c))  // irq 28 to 31 priority register
//#define gpio_portf_dir_r        (*((volatile unsigned long *)0x40025400))
//#define gpio_portf_is_r         (*((volatile unsigned long *)0x40025404))
//#define gpio_portf_ibe_r        (*((volatile unsigned long *)0x40025408))
//#define gpio_portf_iev_r        (*((volatile unsigned long *)0x4002540c))
//#define gpio_portf_im_r         (*((volatile unsigned long *)0x40025410))
//#define gpio_portf_ris_r        (*((volatile unsigned long *)0x40025414))
//#define gpio_portf_icr_r        (*((volatile unsigned long *)0x4002541c))
//#define gpio_portf_afsel_r      (*((volatile unsigned long *)0x40025420))
//#define gpio_portf_pur_r        (*((volatile unsigned long *)0x40025510))
//#define gpio_portf_pdr_r        (*((volatile unsigned long *)0x40025514))
//#define gpio_portf_den_r        (*((volatile unsigned long *)0x4002551c))
//#define gpio_portf_amsel_r      (*((volatile unsigned long *)0x40025528))
//#define gpio_portf_pctl_r       (*((volatile unsigned long *)0x4002552c))
//#define sysctl_rcgc2_r          (*((volatile unsigned long *)0x400fe108))
//#define sysctl_rcgc2_gpiof      0x00000020  // port f clock gating control
////systick
//#define gpio_portf_data_r       (*((volatile unsigned long *)0x400253fc))
//#define gpio_portf_dir_r        (*((volatile unsigned long *)0x40025400))
//#define gpio_portf_afsel_r      (*((volatile unsigned long *)0x40025420))
//#define gpio_portf_den_r        (*((volatile unsigned long *)0x4002551c))
//#define gpio_portf_amsel_r      (*((volatile unsigned long *)0x40025528))
//#define gpio_portf_pctl_r       (*((volatile unsigned long *)0x4002552c))
//#define sysctl_rcgc2_r          (*((volatile unsigned long *)0x400fe108))
//#define nvic_sys_pri3_r         (*((volatile unsigned long *)0xe000ed20))  // sys. handlers 12 to 15 priority
//#define nvic_st_ctrl_r          (*((volatile unsigned long *)0xe000e010))
//#define nvic_st_reload_r        (*((volatile unsigned long *)0xe000e014))
//#define nvic_st_current_r       (*((volatile unsigned long *)0xe000e018))
	
#define ON_BOARD_LED						(*((volatile unsigned long *)0x40025038))

// 2. Declarations Section
//   Global Variables
unsigned long In;  // input from PF4
unsigned long Out; // outputs to PF3,PF2,PF1 (multicolor LED)

//   Function Prototypes
void EnableInterrupts(void);
void WaitForInterrupt(void);  // low power mode
void EdgeCounter_Init(void);
void sawtoothWave(int delay);
void triangleWave(int delay); 
void squareWave(int delay); 
volatile unsigned long Counts = 0;
volatile unsigned long FallingEdges = 0;

// TODO: INCLUDE PLL 	

void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
}


void Systick_1_Second(int seconds){
	unsigned long i;
	for (i = 0; i <= seconds;  i++){
		WaitForInterrupt();
	}
}

	// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){
	Counts = 0;
	FallingEdges = 1;
  //PortF_Init();        						// Call initialization of port PF4 PF2    
	SysTick_Init(16000000);        		//initialize SysTick timer
	EdgeCounter_Init();
	EnableInterrupts();          		 	//AFTER inits, should be global
  while(1){
    Systick_1_Second(FallingEdges);	//delay using systick
		ON_BOARD_LED ^= 0x04; 					// toggle blue led on tm4c
  }
}

void sawtoothWave(int delay){
	unsigned int i; 
	for (i = 0; i < 256; i++){
		GPIO_PORTB_DATA_R +=1 ; 
		//TODO: INCLUDE DELAY
	}
}

void triangleWave(int delay){
	unsigned int i; 
	for (i = 0; i < 256; i++){
		GPIO_PORTB_DATA_R += 1; 
		//TODO: INCLUDE DELAY
	}
	for (i = 256; i>0; i--){
		GPIO_PORTB_DATA_R -= 1; 
		//TODO: INCLUDE DELAY
	}
}

void squareWave (int delay){
	GPIO_PORTB_DATA_R = 0xFF; 
	//TODO: INCLUDE DELAY
	GPIO_PORTB_DATA_R = 0x00; 
	//TODO: INCLUDE DELAY
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

// Subroutine to wait 0.1 sec
// Inputs: None
// Outputs: None
// Notes: ...

	
void SysTick_Handler(void){
	//Nothing important - no handling
  //GPIO_PORTF_DATA_R ^= 0x04;       // toggle PF2
  //Counts = Counts + 1;
}

// global variable visible in Watch window of debugger
// increments at least once per button press
void EdgeCounter_Init(void){      
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
  FallingEdges = 0;             // (b) initialize counter
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

void Init_PortB(void){ // needs 3 more outputs
	unsigned int delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) B clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTB_CR_R = 0x3F;           // allow changes to PB5-0      
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R = 0x3F;          // 5) PB5-PB0 output 
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTB_PUR_R = 0x00;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTB_DEN_R = 0x3F;          // 7) enable digital pins P5-PB0   
																		//0011.1111
}

void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
	FallingEdges += 1;
}

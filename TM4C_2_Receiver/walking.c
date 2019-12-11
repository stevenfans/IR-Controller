/*
	Animation
	An implimentation of helpful fixed point routines
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "walking.h"
#include "ST7735.h"



/**************ST7735_XYPlotMan***************
 Function the draw the man
 Inputs:  x   x co-ordinate of the figure
          y   y co-ordinate of the figure
          h   height of the figure
          color:   color to be given 
 Outputs: none
*/

extern int is_setup;
uint32_t x = 40, ht = 0;	
int32_t l = 15, h = 15;
int j;

void ST7735_XYPlotMan(int32_t x, int32_t y,int32_t h, uint16_t color)
{
		ST7735_DrawCircle(x - 20, 65, 15,color);							//face
		ST7735_DrawLine(x - 20, 80, x - 20, 125, color);			//body
		ST7735_DrawLine(x - 20, 125, x - 20 - y, 150, color);	//leg1
		ST7735_DrawLine(x - 20, 125, x - 20 + y, 150, color);	//leg2
		ST7735_DrawLine(x - 20, 90, x - 20 + h, 100, color);	//hand1
		ST7735_DrawLine(x - 20, 90, x - 20 - h, 105, color);	//hand2
}

void DelayWait10ms(uint32_t n){
	uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}

void walking_animation(int is_setup){
	
	if (!is_setup){
		ST7735_FillScreen(ST7735_BLACK);
		// DRAW THE GROUND
		ST7735_FillScreen(ST7735_CYAN);
		ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
			
		// DRAW THE SUN
		ST7735_FillCircle(100, 30, 15,ST7735_YELLOW);
		is_setup = 1;
	}
	
	// DRAW THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_RED);
	
  DelayWait10ms(10);
		
	// REMOVE THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_CYAN);
 
	// MOVE THE COORDINATES OF THE MAN
	x++;
 
	// CODE for MOVING the LEG
	l--;
	if (l == -15)
	    l = 15;
 
	// CODE for MOVING the HAND    
	if (ht == 1)
	    h++;
	else
	    h--;
 
	if (h == 15)
	   ht = 0;
	else if (h == -15)   
		ht = 1;
 
	//Reset parameters
	  if(x>170){
			x = 40, ht = 0,l = 15, h = 15;
    }
}


void waving_animation(void){	
	x = 85; // center of screen
	if (!is_setup){
		
		// DRAW THE GROUND
		ST7735_FillScreen(ST7735_CYAN);
		ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
			
		// DRAW THE SUN
		ST7735_FillCircle(25, 30, 15,ST7735_YELLOW);
		
		// DRAW THE DOOR
		ST7735_FillRect(80, 80, 120, 168, ST7735_BLACK); // x,y,w,h
			
		is_setup = 1;		
	}
	
	// DRAW THE MAN
		ST7735_DrawCircle(x - 20, 65, 15,ST7735_RED);							//face
		ST7735_DrawLine(x - 20, 80, x - 20, 125, ST7735_RED);			//body
		ST7735_DrawLine(x - 20, 125, x - 20 - l, 150, ST7735_RED);	//leg1
		ST7735_DrawLine(x - 20, 125, x - 20 + l, 150, ST7735_RED);	//leg2
		ST7735_DrawLine(x - 20, x + 20, x - 20 + h, x - 20 + h, ST7735_RED);	//right arm
		
	
	
  DelayWait10ms(5);
		
	// REMOVE THE MAN's hand
		ST7735_DrawLine(x - 20, x + 20, x - 20 + h, x - 20 + h, ST7735_CYAN);	//right arm
 
	
	// CODE for MOVING the HAND    
	if (ht == 1)
	    h++;
	else
	    h--;
 
	if (h == 15)
	   ht = 0;
	else if (h == -15)   
		ht = 1;
}



void car_animation(void){	
	//x = 40; // center of screen
	if (!is_setup){
		
		// DRAW THE GROUND
		ST7735_FillScreen(ST7735_CYAN);
		ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
			
		is_setup = 1;		
	}
	
	// DRAW THE CAR
	ST7735_FillRect(x - 25, 10, 55, 15, ST7735_RED); // body
	ST7735_FillCircle(x - 25, 30, 5,ST7735_BLACK); // rear wheel
	ST7735_FillCircle(x + 25, 30, 5,ST7735_BLACK); // front wheel
	
		
  DelayWait10ms(5);
	
	// REMOVE THE CAR
	ST7735_FillRect(x - 25, 10, 55, 15, ST7735_CYAN); // body
	ST7735_FillCircle(x - 25, 30, 5,ST7735_CYAN); // rear wheel
	ST7735_FillCircle(x + 25, 30, 5,ST7735_CYAN); // front wheel
	
	x++;

	//Reset parameters
	if(x>170){
		x = 40;
	}
	
}


void face_animation(void){	
	if (!is_setup){
		
		// DRAW THE GROUND
		ST7735_FillScreen(ST7735_CYAN);
		ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
		
		// DRAW THE FACE
		ST7735_FillCircle(x - 20, 30, 15,ST7735_BLACK); // LEFT EYE
		ST7735_FillCircle(x + 20, 30, 15,ST7735_BLACK); // RIGHT EYE
		
		is_setup = 1;
	}
	
	// DRAW MOUTH
	ST7735_FillRect(x - 10, 50,40, h, ST7735_BLACK);  // MOUTH
	
  DelayWait10ms(10);
	
	// REMOVE THE MOUTH
	ST7735_FillRect(x - 10, 50,40, h, ST7735_CYAN);  // MOUTH
 
	
	// CODE for MOVING the MOUTH
	if (ht == 1)
	    h++;
	else
	    h--;
 
	if (h == 25)
	   ht = 0;
	else if (h == 5)   
		ht = 1;
}

void clear_animation(void){	
	ST7735_FillScreen(ST7735_BLACK);
}



void walking_backwards_animation(int is_setup){
	
	if (!is_setup){
		ST7735_FillScreen(ST7735_BLACK);
		// DRAW THE GROUND
		ST7735_FillScreen(ST7735_CYAN);
		ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
			
		// DRAW THE SUN
		ST7735_FillCircle(100, 30, 15,ST7735_YELLOW);
		is_setup = 1;
	}
	
	// DRAW THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_RED);
	
  DelayWait10ms(10);
		
	// REMOVE THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_CYAN);
 
	// MOVE THE COORDINATES OF THE MAN
	x--;
 
	// CODE for MOVING the LEG
	l--;
	if (l == -15)
	    l = 15;
 
	// CODE for MOVING the HAND    
	if (ht == 1)
	    h++;
	else
	    h--;
 
	if (h == 15)
	   ht = 0;
	else if (h == -15)   
		ht = 1;
 
	//Reset parameters
	  if(x>170){
			x = 40, ht = 0,l = 15, h = 15;
    }
}



void running_backwards_animation(int is_setup){
	
	if (!is_setup){
		ST7735_FillScreen(ST7735_BLACK);
		// DRAW THE GROUND
		ST7735_FillScreen(ST7735_CYAN);
		ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
			
		// DRAW THE SUN
		ST7735_FillCircle(100, 30, 15,ST7735_YELLOW);
		is_setup = 1;
	}
	
	// DRAW THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_RED);
	
  DelayWait10ms(3);
		
	// REMOVE THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_CYAN);
 
	// MOVE THE COORDINATES OF THE MAN
	x--;
 
	// CODE for MOVING the LEG
	l--;
	if (l == -15)
	    l = 15;
 
	// CODE for MOVING the HAND    
	if (ht == 1)
	    h++;
	else
	    h--;
 
	if (h == 15)
	   ht = 0;
	else if (h == -15)   
		ht = 1;
 
	//Reset parameters
	  if(x>170){
			x = 40, ht = 0,l = 15, h = 15;
    }
}


void running_animation(int is_setup){
	
	if (!is_setup){
		ST7735_FillScreen(ST7735_BLACK);
		// DRAW THE GROUND
		ST7735_FillScreen(ST7735_CYAN);
		ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
			
		// DRAW THE SUN
		ST7735_FillCircle(100, 30, 15,ST7735_YELLOW);
		is_setup = 1;
	}
	
	// DRAW THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_RED);
	
  DelayWait10ms(3);
		
	// REMOVE THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_CYAN);
 
	// MOVE THE COORDINATES OF THE MAN
	x++;
 
	// CODE for MOVING the LEG
	l--;
	if (l == -15)
	    l = 15;
 
	// CODE for MOVING the HAND    
	if (ht == 1)
	    h++;
	else
	    h--;
 
	if (h == 15)
	   ht = 0;
	else if (h == -15)   
		ht = 1;
 
	//Reset parameters
	  if(x>170){
			x = 40, ht = 0,l = 15, h = 15;
    }
}
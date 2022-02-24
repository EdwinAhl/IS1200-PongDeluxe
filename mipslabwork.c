/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */


#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <string.h>


int mytime = 0x5957;
int prime = 1234567;


char textstring[] = "";
uint8_t display[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void set_pixel(int x, int y){
  display[x*2 + (y+1)/8] = 1 << (y % 8);
}


int timeoutcount = 0;

/* Interrupt Service Routine */
void user_isr( void )
{
  if (IFS(0) & 0b100000000) { // if interrupt is timer2
    IFS(0) = IFS(0) & ~0b100000000; // bit 8 resets timer2
    TMR2 = 0; // reset timer for timer2

    if (timeoutcount++ == 10) {
      //time2string ( textstring, mytime );
      //display_string( 3, textstring );
      //display_update();
      tick ( &mytime );
      timeoutcount = 0;
    }
  }

  //only uses two interrupts, has to be switches
  if (IFS(0) & 0b1000000000000000){ 
    IFS(0) = IFS(0) & ~0b1000000000000000; // bit 15 resets INT3
    PORTE++; //increase 
  }
}


/* Lab-specific initialization goes here */
void labinit( void )
{
  // T2
  T2CON = 0b1000000001110000; 
  PR2 = 31250;  // (80M*10^6)/256/10

  // buttons
  TRISD = 0b111111100000; //bits 5-11 inputs
  PORTD = 0;
  
  //lights
  TRISE = 0; // lights are outputs
  PORTE = 0; // lights off for now

  // timing interrupts
  IEC(0) = IEC(0) | 0b100000000;
  IPC(2) = IPC(2) | 0b11100;  
  
  enable_interrupt();
}


/* This function is called repetitively from the main program */
void labwork( void )
{
  display_image(0, display);

  // intializing buttons and switches as variables
  int buttons = getbtns();

  // button 1
  if((buttons & 0b1) == 1) { set_pixel(1,1); }
  
  // button 2
  if((buttons & 0b10) == 2) { set_pixel(2,2); }

  // button 3
  if((buttons & 0b100) == 4) { set_pixel(3,3); }

  // button 4
  if((buttons & 0b1000) == 8) { set_pixel(4,4); }

  /*
  int i = 0;
  for (i = 0; i<=31; i++)
    set_pixel(i,i);
  */
}
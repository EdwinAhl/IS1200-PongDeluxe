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
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void set_pixel(int x, int y){
  display[x + y*32] =  1 << (8 - ( x % 8) );
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
      display_update();
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
  
  //lights
  PORTE = 0; // lights off for now

  // timing interrupts
  IEC(0) = IEC(0) | 0b100000000;
  IPC(2) = IPC(2) | 0b11100;  
<<<<<<< HEAD

  // sw3 interrupt
  IEC(0) = IEC(0) | 0b1000000000000000; // bit 15 enable INT3
  IPC(3) = IPC(3) | 0b11100000000000000000000000000; // bit 26-28 priority for INT3 external interrupt 3
=======
  
  display[1] = 225;
>>>>>>> 7635d1d1e37e980695292ad6b17e437fdc7f0ab8

  enable_interrupt();
  set_pixel(0,0);
}


/* This function is called repetitively from the main program */
void labwork( void )
{
<<<<<<< HEAD
  display_image(0, display);
  //prime = nextprime ( prime );
  //display_string ( 0, itoaconv ( prime ) );
  //display_update();
=======
  // intializing buttons and switches as variables
  int buttons = getbtns();
  int switches = getsw();

  // button 1
  if((buttons & 0b01) == 1) { PORTE = 0b1; }

  // button 2
  if((buttons & 0b10) == 2) { PORTE = 0b10; }

  // button 3
  if((buttons & 0b100) == 4) { PORTE = 0b11; }

  // button 4
  if((buttons & 0b1000) == 8) { PORTE = 0b100; }
>>>>>>> 7635d1d1e37e980695292ad6b17e437fdc7f0ab8
}
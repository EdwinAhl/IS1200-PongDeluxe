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


char textstring[] = "";
uint8_t display[4][128] = {
  {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
  {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  },
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  },
};

void clear_display() {
  int i, j;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 128; j++)
      display[i][j] = 0;
}

void set_pixel(int x, int y) {
  display[x/32][x + (y/8)*32] = display[x/32][x + (y/8)*32] | 1 << (y % 8);
}


int timeoutcount = 0; // used to keep track of time loops

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
  // button 1
  TRISF = 0b10; // second bit in F
  PORTF = 0; // value of button 1 zero to begin with

  // buttons 2-4
  TRISD = 0b111100000; // bits 5-7 buttons 2-4 as inputs (bits 9-11 is for switches)
  PORTD = 0; // value of buttons 2-4 zero to begin with
  
  // lights
  TRISE = 0; // lights are outputs
  PORTE = 0; // lights off for now

  // T2
  T2CON = 0b1000000001110000; // timer on with PRE 256
  PR2 = 31250;  // (80M*10^6)/256/10

  // timing interrupts
  IEC(0) = IEC(0) | 0b100000000;
  IPC(2) = IPC(2) | 0b11100;  
  
  enable_interrupt(); // enable global interrupts

  menu(); // start with
}


// init menu variable
char currentScreen = 'm'; 

/* menu with
  1. Start game
  2. Leadearboard
*/ 
void menu() {
  display_string(0, "1. Start");
  display_string(1, "2. Leaderboard");
  display_string(3, "3. Debug");
  display_update();
}


// buttons

void button1() {
  if (currentScreen == 'm')
    set_pixel(1,1);
  
}

void button2() {
  if (currentScreen == 'd')
    set_pixel(2,2);
}

void button3() {
  if (currentScreen == 'm')
    currentScreen = 'd';

  else if (currentScreen == 'd')
    set_pixel(3,3);
}

void button4() {
  if (currentScreen == 'm')
    set_pixel(4,4);
}


/* This function is called repetitively from the main program */
void labwork( void )
{
  display_image(display);

  // intializing buttons and switches as variables
  int buttons = getbtns();
  int switches = getsw();

  if(switches>0) { clear_display(); }
  
  if(buttons & 0b1) { button1(); } // button 1
  if(buttons & 0b10) {  button2(); } // button 2
  if(buttons & 0b100) {  button3(); } // button 3
  if(buttons & 0b1000) {  button4(); } // button 4

  //test();
}

void test() {
  int i = 0;
    for (i = 0; i<=31; i++)
      set_pixel(i,i);
}
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
#include <string.h>   // ??


/*//////////////////////////////////////////////////////////////////////////////////////////////////
  TIME
*///////////////////////////////////////////////////////////////////////////////////////////////////

int totaltimeout = 0; // global timer
int timeoutcount = 0; // used to keep track of number of time loops

/* Interrupt Service Routine */
void user_isr( void )
{
  if (IFS(0) & 0b100000000) { // if interrupt is timer2
    IFS(0) = IFS(0) & ~0b100000000; // bit 8 resets timer2
    TMR2 = 0; // reset timer for timer2
    totaltimeout++;

    // reset timeout
    if (timeoutcount++ == 10) 
      timeoutcount = 0;
  }

  //only uses two interrupts, has to be switches
  if (IFS(0) & 0b1000000000000000){ 
    IFS(0) = IFS(0) & ~0b1000000000000000; // bit 15 resets INT3
    PORTE++; //increase 
  }
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  DISPLAY
*///////////////////////////////////////////////////////////////////////////////////////////////////

// four 32x32 pixel display, in total 128x32
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


// clears display by setting every value to 0
void clear_display() {
  int i, j;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 128; j++) 
      display[i][j] = 0;
  display_image(display);
}


// sets pixel on dislay to an x,y value 
void set_pixel(int x, int y) {
  // needs explaination...
  display[x/32][(x%32) + (y/8)*32] = display[x/32][x + (y/8)*32] | 1 << (y % 8);  
}


float ball_values[2]; // x,y coordinate
int last_display_ball = 0; 

// WIP... handles ball coordinates and velocity
void display_ball(float x, float y)
{
  // if balls new coordinates is withing limit
  if ((x>=0 && x<=127) && (y>=0 && y<=31) && (totaltimeout - last_display_ball > 0)) {
    clear_display(); // clear display to reset
    set_pixel(x, y);
    ball_values[0] = x;
    ball_values[1] = y;
    last_display_ball = totaltimeout;
  }
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  SCREEN
*///////////////////////////////////////////////////////////////////////////////////////////////////

// init menu variable
char currentScreen; 


// menu screen, number corresponds to button 
void menu() {
  currentScreen = 'm'; // in menu
  display_string(0, "1. Start");
  display_string(1, "2. Leaderboard");
  display_string(3, "3. Debug");
  display_update();
}


// debug  screen
int last_debug = 0;
void debug() {
  currentScreen = 'd';
  clear_display();
  display_ball(64, 15);

  //test();
}

// testing function for debug
void test() {
  int i = 0;
    for (i = 0; i<=31; i++)
      set_pixel(i,i);
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  IO
*///////////////////////////////////////////////////////////////////////////////////////////////////

// BTN1
void button1() {

  // debug
  if (currentScreen == 'd')
    display_ball(ball_values[0], ball_values[1]+1); // move ball in +x in debug
}

// BTN2
void button2() {

  // debug
  if (currentScreen == 'd')
    display_ball(ball_values[0], ball_values[1]-1); // move ball in -x in debug
}

// BTN3
void button3() {
  
  //menu
  if (currentScreen == 'm') {
    last_debug = totaltimeout; 
    debug();
  }

  // debug
  else if (currentScreen == 'd' && (totaltimeout-last_debug > 2)) {
    display_ball(ball_values[0]+1, ball_values[1]); // move ball in +y in debug
  }
}

// BTN4
void button4() {

  // debug
  if (currentScreen == 'd') { // moves ball in -y if in debug 
    display_ball(ball_values[0]-1, ball_values[1]);
  }
}

// SW1
void switch1() {

  // debug
  if (currentScreen == 'd') // goes to menu if in debug
    menu();
}


/* This function is called repetitively from the main program */
void labwork( void )
{
  // display image if not in menu
  if (currentScreen != 'm')
    display_image(display);

  // intializing buttons and switches as variables
  int buttons = getbtns();
  int switches = getsw();

  if(switches>0) { switch1(); } // switch 1

  if(buttons & 0b1) { button1(); } // button 1
  if(buttons & 0b10) {  button2(); } // button 2
  if(buttons & 0b100) {  button3(); } // button 3
  if(buttons & 0b1000) {  button4(); } // button 4
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
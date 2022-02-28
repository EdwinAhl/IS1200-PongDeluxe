/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING 
*/


#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

//#include <string.h>   // ??



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
  //display_image(display); 
}


// sets pixel on dislay to an x, y value
// Only 0 <= x <= 127 working
// Only 0 <= y <= 31 working
void set_pixel(int x, int y) {
  /* 
    display is 4 32x32 cubes aligned horizontally, hence we have a 2d array with the cubes.
    [x/32] picks the correct cube based on x coordinates
    
    The cube itself is a collection of 1x8 (8 pixels tall) columns aligned akin to text (LTR going down and left at newline)
    Each pixel is set by a bit, eg 0000_0001 sets the upper most pixel.
    x%32 selects the X coordinate in the cube, modulus as to make it work with multiple cubes

    + (y/8)*32 is an additional offset y wise. The cubes have 4 rows, if y = 10 
    then it'd use the second row and therefore adds 32 to the index (32 columns per row / 32 ints per row)
    display[x/32][x%32 + (y/8)*32] | allow for setting bits in the same column, otherwise it'd overwrite the bits.
    1 << (y % 8)
    This sets the pixel correctly in the column, y = 2 => 0000_0100, y = 5 => 0010_0000
    Modulus since there's multiple columns. 
  */
  display[x/32][x%32 + (y/8)*32] = display[x/32][x%32 + (y/8)*32] | 1 << (y % 8);  
}

// Caps the value fron the input based on start and end.
// Used in display_ball to not overshoot the ball.
float getBetween(float input, int start, int end){
  if (input > end){
    return end;
  } else if (input < start) {
    return start;
  } else return input;
}

int floor(float input) {
  return (int) input;
}
int ceil(float input) {
  return (int) (input + 1);
}

// Middle value as start position // TODO make a reset_ball method for replaying
float ball_x = 63.5f; // 0 <= x <= 127
float ball_y = 15.5f; // 0 <= y <= 31

// WIP... handles ball coordinates and velocity
void display_ball() {

  // Makes sure the ball is within the screen.
  ball_x = getBetween(ball_x, 127, 0);
  ball_y = getBetween(ball_y, 127, 0);

  clear_display(); // reset screen, //TODO PLACE SOMEWHERE ELSE

  // Creates 4 pixels for the ball based on the center point.
  set_pixel(floor(ball_x), floor(ball_y));
  set_pixel(floor(ball_x), ceil(ball_y));

  set_pixel(ceil(ball_x), ceil(ball_y));
  set_pixel(ceil(ball_x), floor(ball_y));

  //last_display_ball = totaltimeout;
  display_image(display); //TODO PLACE SOMEWHERE ELSE
}


// paddle values
const float paddle_x = 10;
float paddle1_y = 15.5f;
float paddle2_y = 15.5f;

// handles both paddles coordinates and velocity
display_paddle() {
  
  // padle1
  set_pixel(paddle_x, paddle1_y+2);
  set_pixel(paddle_x, paddle1_y+1);
  set_pixel(paddle_x, paddle1_y+0);
  set_pixel(paddle_x, paddle1_y-1);
  set_pixel(paddle_x, paddle1_y-2);

  // padle2
  set_pixel(127 - paddle_x, paddle2_y+2);
  set_pixel(127 - paddle_x, paddle2_y+1);
  set_pixel(127 - paddle_x, paddle2_y+0);
  set_pixel(127 - paddle_x, paddle2_y-1);
  set_pixel(127 - paddle_x, paddle2_y-2);
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  SCREEN
*///////////////////////////////////////////////////////////////////////////////////////////////////

// screen state definitions
#define MENU 'm'
#define PLAY 'p'
#define SINGLEPLAYER 'v'
#define MULTIPLAYER 'w'
#define LEADERBOARD 'l'
#define DEBUG 'd'

// init current screen variable
char currentScreen; 
int optionsdelay = 0; // delay for switching between options so same button isn't pressed immediately


// menu screen, number corresponds to button 
void menu() {
  currentScreen = MENU; // in menu
  display_string(0, "--PONG DELUXE--");
  display_string(1, "1. Play");
  display_string(2, "2. Leaderboard");
  display_string(3, "3. Debug");
  display_update();
}


// start screen
void play() {
  currentScreen = PLAY; // in screen
  display_string(0, "1. Singleplayer");
  display_string(1, "2. Multiplayer");
  display_string(2, "3. Back");
  display_string(3, "");
  display_update();
}


// WIP... singleplayer screen
void singleplayer() {
  currentScreen = SINGLEPLAYER; // in singleplayer

  // TEMPORARY
  display_string(0, "Singleplayer");
  display_string(1, "3. Back");
  display_string(2, "");
  display_string(3, "");
  display_update();
}


// WIP... multiplayer screen
void multiplayer() {
  currentScreen = MULTIPLAYER; // in multiplayer

  // TEMPORARY
  display_string(0, "Multiplayer");
  display_string(1, "3. Back");
  display_string(2, "");
  display_string(3, "");
  display_update();
}


// WIP... leaderboard screen
void leaderboard() {
  currentScreen = LEADERBOARD; // in leaderboard

  // TEMPORARY
  display_string(0, "Leaderboard");
  display_string(1, "3. Back");
  display_string(2, "");
  display_string(3, "");
  display_update();
}


// testing function for debug, subject to change
void test() {
  int i = 0;
    for (i = 0; i<=31; i++)
      set_pixel(i+96,i);
}

// debug  screen, subject to change
void debug() {

  currentScreen = DEBUG;
  clear_display();

  // test
  display_ball();
  test();
  
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  IO
*///////////////////////////////////////////////////////////////////////////////////////////////////

const int delayvalue = 2; // how much to delay


// BTN1
void button1() {

  // if enough delay has passed
  if ((totaltimeout-optionsdelay) > delayvalue) {

    // menu
    if (currentScreen == MENU) {
      currentScreen = PLAY;
    }

    // play
    else if (currentScreen == PLAY) {
      currentScreen = SINGLEPLAYER;
    }

    optionsdelay = totaltimeout; // reset optionsdelay to present totaltimeout
  } 

  // debug
  if (currentScreen == DEBUG) {
    display_ball(); // move ball in +x in debug
  }
}


// BTN2
void button2() {

  // if enough delay has passed
  if (totaltimeout-optionsdelay > delayvalue) {

    // menu
    if (currentScreen == MENU) {
      currentScreen = LEADERBOARD;
    }

    // play
    else if (currentScreen == PLAY) {
      currentScreen = MULTIPLAYER;
    }

    optionsdelay = totaltimeout; // reset optionsdelay to present totaltimeout
  }

  // debug
  if (currentScreen == DEBUG) {
    display_ball(); // move ball in -x in debug
  }
}


// BTN3
void button3() {

  // if enough delay has passed
  if (totaltimeout-optionsdelay > delayvalue) {

    // menu, go to debug
    if (currentScreen == MENU) { 
      currentScreen = DEBUG;
    }

    // start or leaderboard, go back to menu
    else if (currentScreen == PLAY || currentScreen == LEADERBOARD) {
      currentScreen = MENU;
    }

    // TEMPORARY... singleplayer or multiplayer, go back to play
    else if (currentScreen == SINGLEPLAYER || currentScreen == MULTIPLAYER) {
      currentScreen = PLAY;
    }
    optionsdelay = totaltimeout; // reset optionsdelay to present totaltimeout
  }

  // debug
  if (currentScreen == DEBUG) {
    display_ball(); // move ball in +y in debug
  }
}


// BTN4
void button4() {

  // debug
  if (currentScreen == DEBUG) { // moves ball in -y if in debug 
    display_ball();
  }
}


// SW1
void switch1() {

  // debug
  if (currentScreen == DEBUG) // goes to menu if in debug
    currentScreen = MENU;
}


char oldScreen; // used to determine if user has switches screen

// checks states and starts correct one
void checkstate() {
  
  // won't update screen state if new state is same as old state
  if (oldScreen != currentScreen) {
    oldScreen = currentScreen;
    
    // clears display if not in debug 
    if (currentScreen != DEBUG) {
      clear_display();
    }

    // states
    switch(currentScreen)
    {
      case MENU:
        menu();
        break;
      
      case PLAY:
        play();
        break;

      case SINGLEPLAYER:
        singleplayer();
        break;

      case MULTIPLAYER:
        multiplayer();
        break;

      case LEADERBOARD:
        leaderboard();
        break;
      
      case DEBUG:
        debug();
        break;
    }
  }
}


/* This function is called repetitively from the main program */
void labwork( void )
{
  checkstate(); // checks current state for buttons to act accordingly

  // intializing buttons and switches as variables
  int buttons = getbtns();
  int switches = getsw();

  if(switches > 0) { switch1(); } // switch 1

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

  menu(); // start with menu
}
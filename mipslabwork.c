/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING 
*/



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  CONST
*///////////////////////////////////////////////////////////////////////////////////////////////////

// include
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdio.h>

// screen state definitions
#define MENU 'm'
#define PLAY 'p'
#define SINGLEPLAYER 'v'
#define MULTIPLAYER 'w'
#define LEADERBOARD 'l'
#define SCORE 's'
#define RESULTS 'r'
#define CREDITS 'c'

// screen size definitions
#define SCREEN_WIDTH_FLOAT 127
#define SCREEN_HEIGHT_FLOAT 31

const int rounds_to_win = 3; // rounds to win to win the whole game
int player1_points = 0; // keeps track of total round wins for player1
int player2_points = 0; // keeps track of total round wins for player2
int is_singleplayer = 0; // if gamemode is in singleplayer or multilpayer

char current_screen; // init current screen variable
char old_screen; // used to determine if user has switched screen

char int_to_char(int i) { return '0' + i; } // converts an int to it's corresponding char 


/*//////////////////////////////////////////////////////////////////////////////////////////////////
  TIME
*///////////////////////////////////////////////////////////////////////////////////////////////////

int total_timeout = 0; // global timer
int timeoutcount = 0; // used to keep track of number of time loops
void update_ball_pos_on_velocity(); 
void update_canvas();

/* Interrupt Service Routine */
void user_isr( void )
{
  if (IFS(0) & 0b100000000) { // if interrupt is timer2
    IFS(0) = IFS(0) & ~0b100000000; // bit 8 resets timer2
    TMR2 = 0; // reset timer for timer2
    total_timeout++;

    // updates ball if in a game
    if (current_screen == SINGLEPLAYER || current_screen == MULTIPLAYER) { 
      update_ball_pos_on_velocity(); 
      update_canvas();

      // updates AI if in singleplayer mode
      if (is_singleplayer) {
        ai_update();
      }
    }

    // reset timeout
    if (timeoutcount++ == 10) {
      timeoutcount = 0;
    }
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
  
  // display is 4 32x32 cubes aligned horizontally, hence we have a 2d array with the cubes.
  // [x/32] picks the correct cube based on x coordinates
    
    
  // The cube itself is a collection of 1x8 (8 pixels tall) columns aligned akin to text (LTR going down and left at newline)
  // Each pixel is set by a bit, eg 0000_0001 sets the upper most pixel.

  // x%32 selects the X coordinate in the cube, modulus as to make it work with multiple cubes
  
  // + (y/8)*32 is an additional offset y wise. The cubes have 4 rows, if y = 10 
  // then it'd use the second row and therefore adds 32 to the index (32 columns per row / 32 ints per row)

  // display[x/32][x%32 + (y/8)*32] | allow for setting bits in the same column, otherwise it'd overwrite the bits.

  // 1 << (y % 8)
  // This sets the pixel correctly in the column, y = 2 => 0000_0100, y = 5 => 0010_0000
  // Modulus since there's multiple columns. 
  display[x/32][x%32 + (y/8)*32] = display[x/32][x%32 + (y/8)*32] | 1 << (y % 8);
}


// Caps the value fron the input based on start and end.
// Used in display_ball to not overshoot the ball.
float get_between(float input, int start, int end){
  if (input > end){
    return end;
  } else if (input < start) {
    return start;
  } else return input;
}


// math floor and ceiling
int floor(float input) {
  return (int) input;
}

// Same as math ceil, but doesnt round up above the max. E.g 6.0 => 6.0, 6.1 => 7.0, 
int ceil_custom(float input, float max) {
  if (max <= input + 1) return floor(input);
  return (int) (input + 1);
}


// ball x,y starting value
float ball_x_velocity = 1;
float ball_y_velocity = 1;


// Middle value as start position
float ball_x = 0; // 0 <= x <= 127
float ball_y = 0; // 0 <= y <= 31

void center_ball() {
  ball_x = SCREEN_WIDTH_FLOAT / 2;
  ball_y = SCREEN_HEIGHT_FLOAT / 2;
}

// paddle values, 7 pixles from each side
const float paddle_x = 7;
// This should always be an odd number, otherwise set_new_velocity_on_paddle_collision will fail.
const int paddle_height = 9;
int paddle_middle_height = 4; //(int) ((paddle_height-1) / 2); // 9 => 4

// paddle 
float paddle1_y = 15.5f;
float paddle2_y = 15.5f;

void set_new_velocity_on_paddle_collision() {
  // If ball on right side!
  if (ball_x > (SCREEN_WIDTH_FLOAT / 2) &&
    // If ball is between the paddle and end
    ball_x >= (SCREEN_WIDTH_FLOAT - paddle_x) && ball_x < (SCREEN_WIDTH_FLOAT - paddle_x + ball_x_velocity + 1) && 

    // if ball y is within paddle height
    ball_y < (paddle2_y + paddle_middle_height + 0.5) && 
    ball_y > (paddle2_y - paddle_middle_height - 0.5)
  ) {
    ball_x_velocity = -ball_x_velocity;

  // If ball on left side!
  } else if (ball_x < (SCREEN_WIDTH_FLOAT / 2) &&
    // If ball is between the paddle and start
    ball_x <= paddle_x && ball_x > (paddle_x + ball_x_velocity - 1) && 
    
    // if ball y is within paddle height
    ball_y < (paddle1_y + paddle_middle_height + 0.5) && 
    ball_y > (paddle1_y - paddle_middle_height - 0.5)
  ){
    ball_x_velocity = -ball_x_velocity;
  }
}

// Inverses the velocity on edge bounces.
void set_new_velocity_on_edge() {
  if (ball_x > SCREEN_WIDTH_FLOAT || ball_x < 0) {
    ball_x_velocity = -ball_x_velocity;

    // adds points to a player if they score
    if (ball_x > SCREEN_WIDTH_FLOAT) {
      player1_points++;
    }
    else if (ball_x < 0) {
      player2_points++;
    }

    current_screen = SCORE;
  }
  if (ball_y > SCREEN_HEIGHT_FLOAT || ball_y < 0){
    ball_y_velocity = -ball_y_velocity;
  }
}


// Might need shorter name but this is good enough for now.
void update_ball_pos_on_velocity() {
  ball_x += ball_x_velocity;
  ball_y += ball_y_velocity;
}


// creates the ball and displays it based on it's position
void display_ball() {

  set_new_velocity_on_paddle_collision();
  set_new_velocity_on_edge();  // Sets the new velocity, important that it's called before get_between.
  // Makes sure the ball is within the screen.
  ball_x = get_between(ball_x, 0, SCREEN_WIDTH_FLOAT);
  ball_y = get_between(ball_y, 0, SCREEN_HEIGHT_FLOAT);

  // clear_display(); // reset screen, //TODO PLACE SOMEWHERE ELSE

  // Creates 4 pixels for the ball based on the center point.
  set_pixel(floor(ball_x), floor(ball_y));
  set_pixel(floor(ball_x), ceil_custom(ball_y, SCREEN_HEIGHT_FLOAT));

  set_pixel(ceil_custom(ball_x, SCREEN_WIDTH_FLOAT), ceil_custom(ball_y, SCREEN_HEIGHT_FLOAT));
  set_pixel(ceil_custom(ball_x, SCREEN_WIDTH_FLOAT), floor(ball_y));

  //last_display_ball = totaltimeout;
  // display_image(display); //TODO PLACE SOMEWHERE ELSE
}


// handles both paddles coordinates and velocity
display_paddle() {
  
  // get correct value on paddles
  paddle1_y = get_between(paddle1_y, (paddle_x+1)/2, 31-(paddle_x+1)/2); //(paddle_x+1)/2 for it's with from the middle including bit
  paddle2_y = get_between(paddle2_y, (paddle_x+1)/2, 31-(paddle_x+1)/2);

  // paddle1
  int i = -paddle_middle_height;
  for (i; i < paddle_middle_height+1; i++) {
    int new_paddle_x = paddle_x - (i == -paddle_middle_height || i == paddle_middle_height);
    set_pixel(new_paddle_x, paddle1_y+i);
  }

  // paddle2
  i = -paddle_middle_height;
  for (i; i < paddle_middle_height+1; i++) {
    int new_paddle_x = SCREEN_WIDTH_FLOAT - paddle_x + (i == -paddle_middle_height || i == paddle_middle_height);
    set_pixel(new_paddle_x, paddle2_y+i);
  }
}


// updates AI inputs depending on where ball is, increase it's height 
void ai_update() {

  // only moves defensively when ball is close enough
  if (SCREEN_WIDTH_FLOAT - paddle_x - ball_x < 40) {
    // if ball is lower than paddle
    if(ball_y > paddle2_y) {
      paddle2_y++;
    }

    // if ball is heigher than paddle, lower it's height
    else if (ball_y < paddle2_y) {
      paddle2_y--; 
    }
  }

  // if not centered 
  else if (paddle2_y != 15) {
    if (paddle2_y > 15.5) {
      paddle2_y--;
    }
    else if (paddle2_y < 15.5) {
      paddle2_y++;
    }
  }
    

}


// updates the screen when moving pixles
void update_canvas() {
  clear_display();
  display_paddle();
  display_ball();
  display_image(display);
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  SCREEN
*///////////////////////////////////////////////////////////////////////////////////////////////////

// menu screen, number corresponds to button 
void menu() {
  current_screen = MENU; // in menu
  display_string(0, "--PONG DELUXE--");
  display_string(1, "1. Play");
  display_string(2, "2. Leaderboard");
  display_string(3, "3. Credits");
  display_update();
}


// start screen
void play() {
  current_screen = PLAY; // in screen
  display_string(0, "1. Singleplayer");
  display_string(1, "2. Multiplayer");
  display_string(2, "3. Back");
  display_string(3, "");
  display_update();
}


// WIP... singleplayer screen
void singleplayer() {
  current_screen = SINGLEPLAYER; // in singleplayer
  is_singleplayer = 1;

  // TEMPORARY
  display_string(0, "Singleplayer");
  display_string(1, "3. Back");
  display_string(2, "");
  display_string(3, "");
  display_update();
}


// WIP... multiplayer screen
void multiplayer() {
  current_screen = MULTIPLAYER; // in multiplayer
  is_singleplayer = 0;

  center_ball();
  update_canvas();
}


// WIP... leaderboard screen
void leaderboard() {
  current_screen = LEADERBOARD; // in leaderboard

  // TEMPORARY
  display_string(0, "Leaderboard");
  display_string(1, "3. Back");
  display_string(2, "");
  display_string(3, "");
  display_update();
}


// show current score in between points
void score() {
  current_screen = SCORE;
  
  // if a player has won
  if (player1_points >= rounds_to_win || player2_points >= rounds_to_win) {
    current_screen = RESULTS;
  }
  else {

    // player1 points to char array
    char p1p[] = "P1 =  "; 
    p1p[5] = int_to_char(player1_points);

    // player2 points to char array
    char p2p[] = "P2 =  "; 
    if (is_singleplayer) { // if it should say AI instead of P2
      p2p[0] = 'A';
      p2p[1] = 'I';
    }
    p2p[5] = int_to_char(player2_points);
    
    // scoreboard
    display_string(0, p1p);
    display_string(1, p2p);
    display_string(2, "");
    display_string(3, "3. Continue");
    display_update();
  }

  // reset paddle position
  paddle1_y = 15.5f;
  paddle2_y = 15.5f;
}


// shows which player won after a game
void results () {
  current_screen = RESULTS;
  
  // player1 won game
  if (player1_points >= rounds_to_win) {
    display_string(0, "Player1 won!");
  }

  // player2 or AI won game
  if (player2_points >= rounds_to_win) {
    if (is_singleplayer) {
      display_string(0, "AI won!");
    }
    else {
      display_string(0, "Player2 won!");
    }
  }
  
  // go back
  display_string(1, "");
  display_string(2, "3. Back");
  display_string(3, "");
  display_update();

  // reset points
  player1_points = 0;
  player2_points = 0;
}


//credits
void credits() {
  display_string(0, "By:");
  display_string(1, "Baltzar L");
  display_string(2, "Edwin A");
  display_string(3, "3. Back");
  display_update();
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  IO
*///////////////////////////////////////////////////////////////////////////////////////////////////

int press_delay = 0; // delay for switching between options so same button isn't pressed immediately
const int delay_value_game_inputs = 1; // how much to delay
const int delay_value_menu_inputs = 3;


// BTN1
void button1() {

  // if enough delay has passed for menu switching
  if((total_timeout - press_delay) > delay_value_menu_inputs) {

    // menu
    if (current_screen == MENU) {
      current_screen = PLAY;
    }

    // play
    else if (current_screen == PLAY) {
      current_screen = SINGLEPLAYER;
    }
  }

  // if enough delay has passed for gameplay
  if ((total_timeout - press_delay) > delay_value_game_inputs) {

    // multiplayer
    if (current_screen == MULTIPLAYER) {
      paddle2_y--;
    }

    press_delay = total_timeout; // reset optionsdelay to present totaltimeout
  } 
}


// BTN2
void button2() {

  // if enough delay has passed for menu switching
  if((total_timeout - press_delay) > delay_value_menu_inputs) {
    // menu
    if (current_screen == MENU) {
      current_screen = LEADERBOARD;
    }

    // play
    else if (current_screen == PLAY) {
      current_screen = MULTIPLAYER;
    }
  }

  // if enough delay has passed for gameplay input
  if (total_timeout - press_delay > delay_value_game_inputs) {

    // multiplayer
    if (current_screen == MULTIPLAYER) {
      paddle2_y++;
    }

    press_delay = total_timeout; // reset optionsdelay to present totaltimeout
  }
}


// BTN3
void button3() {

  // if enough delay has passed for menu switching
  if((total_timeout - press_delay) > delay_value_menu_inputs) {

    // menu, go to credits
    if (current_screen == MENU) { 
      current_screen = CREDITS;
    }

    // start, leaderboard or credits, go back to menu
    else if (current_screen == PLAY || current_screen == LEADERBOARD || current_screen == CREDITS) {
      current_screen = MENU;
    }

    // score from singleplayer
    else if (current_screen == SCORE && is_singleplayer) {
      current_screen = SINGLEPLAYER;
    }

    // score from multiplayer
    else if (current_screen == SCORE && !is_singleplayer) { 
      current_screen = MULTIPLAYER;
    }

    // results
    else if (current_screen == RESULTS) {
      current_screen = PLAY;
    }
  }

  // if enough delay has passed for gameplay inputs
  if (total_timeout - press_delay > delay_value_game_inputs) {
    // game
    if (current_screen == SINGLEPLAYER || current_screen == MULTIPLAYER) {
      paddle1_y--;
    }
    
    press_delay = total_timeout; // reset optionsdelay to present totaltimeout
  }
}


// BTN4
void button4() {

  // if enough delay has passed
  if (total_timeout - press_delay > delay_value_game_inputs) {

    // game
    if (current_screen == SINGLEPLAYER || current_screen == MULTIPLAYER) {
      paddle1_y++;
    }
    
    press_delay = total_timeout; // reset optionsdelay to present totaltimeout
  }
}


// SW1
void switch1() {

  // game
  if (current_screen == SINGLEPLAYER || current_screen == MULTIPLAYER) // goes to menu if in game
    current_screen = MENU;
}


// checks states and starts correct one
void checkstate() {
  
  // won't update screen state if new state is same as old state
  if (old_screen != current_screen) {
    old_screen = current_screen;
    
    // clears display if not in a game 
    if (current_screen != SINGLEPLAYER || current_screen != MULTIPLAYER) {
      clear_display();
    }

    // screen states
    switch(current_screen)
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

      case SCORE:
        score();
        break;

      case RESULTS:
        results();
        break;

      case LEADERBOARD:
        leaderboard();
        break;

      case CREDITS:
        credits();
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
  T2CON = 0b1000000001100000; // timer on with PRE 256
  PR2 = 31250;  // (80M*10^6)/256/10

  // timing interrupts
  IEC(0) = IEC(0) | 0b100000000;
  IPC(2) = IPC(2) | 0b11100;  
  
  enable_interrupt(); // enable global interrupts

  menu(); // start with menu
}
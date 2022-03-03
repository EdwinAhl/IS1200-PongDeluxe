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
#define DIFFICULTY 'd'
#define MULTIPLAYER 'w'
#define LEADERBOARD 'l'
#define SCORE 's'
#define RESULTS 'r'
#define CREDITS 'c'
#define WRITE_LEADERBOARD 'b'

// screen size definitions
const SCREEN_WIDTH_FLOAT = 127;
const SCREEN_HEIGHT_FLOAT = 31;

// difficulty definitions
#define EASY 0
#define HARD 1
#define INCREASING 2

// Leaderboard array (only space for 3) 
#define ARRAY_SIZE 3 // size of leaderboard arrays
char leaderboard_names[ARRAY_SIZE+1][ARRAY_SIZE+1];
int leaderboard_scores[ARRAY_SIZE] = {0, 0, 0};
char selected_char = 'A'; // nr 65-90 is capital letters in ASCII 

// game
const int rounds_to_win = 3; // rounds to win to win the whole game
int player1_points = 0; // keeps track of total round wins for player1
int player2_points = 0; // keeps track of total round wins for player2
int is_singleplayer = 0; // if gamemode is in singleplayer or multilpayer

// ai difficulty
int difficulty = EASY;
int ai_reaction_pixels; // interval of pixels to ball ai reacts within
int ai_centers = 0;     // if the AI is allowed to recenter for a defensive position
float speedup = 1.0002; // ball velocity speedup for increasing difficulty

// screen
char current_screen; // init current screen variable
char old_screen; // used to determine if user has switched screen

// delay
const float paddle_y_velocity = 0.5; 
int press_delay = 0; // time when last input
const int delay_value_game_inputs = 1; // how much to delay game inputs
const int delay_value_menu_inputs = 3; // how muhc to delay menu inputs


// paddle values, 7 pixles from each side
const float paddle_x = 7;
const int paddle_height = 9; // This should always be an odd number, otherwise set_new_velocity_on_paddle_collision will fail.
const int paddle_middle_height = 4; //(int) ((paddle_height-1) / 2); // 9 => 4

// calculates half paddle
const float half_paddle = 4.5; // paddle_middle_height + 0.5
const float half_paddle_exp = 20.25; // 4.5²


// paddle 
float paddle1_y = 15.5f;
float paddle2_y = 15.5f;


// ball x,y starting value
float start_velocity = 0.375; // 1.5 / 4
float ball_x_velocity = 1;
float ball_y_velocity = 1;

// ball position
float ball_x = 0; // 0 <= x <= 127
float ball_y = 0; // 0 <= y <= 31


// timers
int total_timeout = 0; // global timer
int game_time = 0; // for singleplayer increasing difficulty
int timeoutcount = 0; // used to keep track of number of time loops


// converts an int to it's corresponding char
char int_to_char(int i) { return '0' + i; }

// returns reversed array of input number
// e.g 1234 => "4321"
// 23 => "32" 
// THIS ONLY RETURNS 4 CHARS MAX, IN REVERSED ORDER.
char* big_int_to_char_array(int number) {
  static char return_value[4] = "    ";
  int i = 1;
  int j = 0;
  for (j; j<4; j++) {
    return_value[j] = ' '; 
  }
  for (i; number > 0; i++) {
    int last_digit = number % 10;
    return_value[i-1] = int_to_char(last_digit);
    number -= last_digit;
    number /= 10;
  }
  return return_value;
}

// returns an array with both name and number in it
char* name_and_points_array(char* name, int points) {
  static char return_value[8];
  
  char* points_array = big_int_to_char_array(points);

  
  // name
  int i = 0; // position of name 
  for (i; i<=2; i++) {
    if (name[i] != '\0') {
    return_value[i] = name[i]; // name
    }
    else {
      return_value[i] = ' ';
    }
  }

  // points
  i = 3;
  for(i; i<7; i++) {
    if (points_array[6-i] != '\0')
      return_value[i] = points_array[6-i];
    else {
      return_value[i] = ' ';
    }
  }
  //return_value[0] = int_to_char(2);
  return return_value; // return "name points"
}

// functions
void update_ball_pos_on_velocity(); 
void update_canvas();
void ai_update();



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  TIME
*///////////////////////////////////////////////////////////////////////////////////////////////////

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
    if (timeoutcount++ == 40) {
      timeoutcount = 0;

      // updates AI if in singleplayer mode
      if (is_singleplayer) {
        game_time++;
      }
    }
  }

  //only uses two interrupts, has to be switches
  if (IFS(0) & 0b1000000000000000){ 
    IFS(0) = IFS(0) & ~0b1000000000000000; // bit 15 resets INT3
    PORTE++; //increase 
  }
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  MATH
*///////////////////////////////////////////////////////////////////////////////////////////////////

// Caps the value fron the input based on start and end, used in display_ball to not overshoot the ball.
float get_between(float input, int start, int end){
  if (input > end){
    return end;
  } else if (input < start) {
    return start;
  } else return input;
}


// math floor and ceiling
int floor_custom(float input) {
  return (int) input;
}

// Same as math ceil, but doesnt round up above the max. E.g 6.0 => 6.0, 6.1 => 7.0, 
int ceil_custom(float input, float max) {
  if (max <= input + 1) return floor_custom(input);
  return (int) (input + 1);
}


// Using the Babylonian method 
// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method
float sqrt(float number){
  float current_number = number / 2; // Start estimate close to the real root
  const int iterations = 15; // loop 15 times
  int i;
  for (i = 0; i < iterations; i++){
	  current_number = (current_number + number / current_number) / 2;
  }
  return current_number;
}


// gets absolute value of a float number
float abs(float number) {
  return number < 0 ? -number : number;
}


// The elipsis is calculated with f(x), this function solves f'(x) for the elipsis.
// Used to calculate the reflection normal. 

// Half of paddle_height = 4.5
// f(x) = sqrt(4.5² - 4.5²x²)
// f'(x) = ...
float calculate_derivative(float intercept_x){
  float derivative = (half_paddle_exp * intercept_x) / 
    sqrt(-half_paddle_exp * intercept_x * intercept_x + half_paddle_exp);

  return derivative;
}

// x² + px + q = 0
// Solve for x
// lower will give the smaller result if true
float pq_formula(float p, float q, int lower){
  float diff = sqrt(((p*p) / 4) - q);
  float realdiff = lower ? -diff : diff;
  return -(p / 2) + realdiff;
}

// ax² + bx + c = 0
// Solve for x
float abc_formula(float a, float b, float c, int lower){
  float p = b/a;
  float q = c/a;
  return pq_formula(p, q, lower);
}


// This function calculates the relative x-coordinate the ball hits the epipsis at.
// Used to calculate the derivative.

// sum = pos_y - slope * pos_x
// f(x) = slope * x + sum
// Solve for x
float calculate_intercept_x(float slope, int is_ball_left, float distance_from_paddle_x, float distance_from_paddle_y){
  // halft paddle
  float sum = distance_from_paddle_y - slope * distance_from_paddle_x;

  // Solve sqrt(4.5²-4.5²x²) = slope * x + sum
  float intercept_x = 
  abc_formula(
    abs(half_paddle_exp + slope * slope),
    2 * slope * sum,
    sum * sum - half_paddle_exp,
    is_ball_left
  );
  return intercept_x;
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


// centers ball between rounds
void center_ball() {
  ball_x = SCREEN_WIDTH_FLOAT / 2;
  ball_y = SCREEN_HEIGHT_FLOAT / 2;
  ball_x_velocity = start_velocity;
  ball_y_velocity = start_velocity;

  // This is used to normalize the new velocity as to make it 1 again
  // float base_velocity = sqrt(2 * (start_velocity * start_velocity));
  // float normal = sqrt(ball_x_velocity * ball_x_velocity + ball_y_velocity * ball_y_velocity);
  // float speed_multiplier = base_velocity / normal;

  // Favors x velocity.
  /*
  float tmp_x_vel = ball_x_velocity;
  int x_multiplier = tmp_x_vel < 0 ? -1 : 1;
  int y_multiplier = ball_y_velocity < 0 ? -1 : 1;

  if (abs(ball_y_velocity) > abs(ball_x_velocity)) {
    ball_x_velocity = ball_y_velocity; // * speed_multiplier;
    ball_y_velocity = tmp_x_vel; //* speed_multiplier;
  }*/
}


// This sets the new mirrored velocity based on 
void calculate_reflection_and_set_velocity(){

  // If the ball is left of the paddle. Eg if it's on the right side of the
  // screen it's left of the paddle, 5 ticks backwards.
  int is_ball_left = ball_x > SCREEN_WIDTH_FLOAT / 2;
  int is_ball_left_multiplier = is_ball_left ? 1 : -1; // Positive to the left, negative to the right
  float distance_from_paddle_x = is_ball_left ? -(SCREEN_WIDTH_FLOAT - paddle_x - ball_x) : ball_x - paddle_x;

  // If ball is on the upper side of the paddle.
  float current_paddle_y = is_ball_left ? paddle2_y : paddle1_y;
  float distance_from_paddle_y = (current_paddle_y - ball_y);

  // calculate slope and intercept
  float slope = -ball_y_velocity / ball_x_velocity;

  // Relative values to the paddle.
  float intercept_x = calculate_intercept_x(slope, is_ball_left, distance_from_paddle_x, distance_from_paddle_y);
  float intercept_y = -(distance_from_paddle_y + (intercept_x - distance_from_paddle_x) * slope);
  int is_ball_upper = intercept_y < 0;
  
  // --- Calculate the mirror with the normal and an incoming vector ---
  float normal_vector_x = calculate_derivative(intercept_x);

  // used for other caluclations to get correct positive/negative value on coordinates for ball
  int is_ball_upper_multiplier = is_ball_upper ? 1 : -1; // Positive above, negative below
  float normal_vector_y = is_ball_upper_multiplier;

  // reflection vector
  float reflection_vector_x = 1;
  float reflection_vector_y = slope; // slope is velocity angle 

  // reflection = 𝟐𝒑𝒓𝒐𝒋_𝒗⃗⃗⃗(𝒖⃗⃗⃗) − 𝒖
  float base_reflection = 2 * 
    // U * V
    (normal_vector_x * reflection_vector_x + normal_vector_y * reflection_vector_y) /
    // div ||v||²
    (normal_vector_x * normal_vector_x + normal_vector_y * normal_vector_y);

  /*
  printf("intercept_x %f\n", intercept_x);
  printf("intercept_y %f\n", intercept_y);
  printf("is_ball_upper %d\n", is_ball_upper);
  */

  // These can be used but leads to unpredictable speeds
  float base_ball_x_velocity = -is_ball_left_multiplier * (base_reflection * normal_vector_x - reflection_vector_x);
  float base_ball_y_velocity = is_ball_left_multiplier * (base_reflection * normal_vector_y - reflection_vector_y);


  // This is used to normalize the new velocity as to make it the same total speed
  float reflection_normal = sqrt(base_ball_x_velocity * base_ball_x_velocity + base_ball_y_velocity * base_ball_y_velocity);
  float current_normal = sqrt(ball_x_velocity * ball_x_velocity + ball_y_velocity * ball_y_velocity);

  //printf("base_ball_x_velocity %f\n", (base_ball_x_velocity / reflection_normal));
  //printf("base_ball_y_velocity %f\n\n", (base_ball_y_velocity / reflection_normal));

  // changes ball velocity based on reflection
  ball_y_velocity = (base_ball_y_velocity / reflection_normal) * current_normal;
  ball_x_velocity = (base_ball_x_velocity / reflection_normal) * current_normal;
}

// sets new veclocity based on where ball hit the paddle
void set_new_velocity_on_paddle_collision() {
  const int paddle_width = 1;
  // If ball on right side!
  if (ball_x > (SCREEN_WIDTH_FLOAT / 2) &&
    
    // Checks if to the right of the paddle
    ball_x >= (SCREEN_WIDTH_FLOAT - paddle_x - paddle_width) &&
    // Checks if to the left of the paddle
    ball_x <= (SCREEN_WIDTH_FLOAT - paddle_x + paddle_width) &&

    // if ball y is within paddle height
    ball_y <= (paddle2_y + half_paddle) && 
    ball_y >= (paddle2_y - half_paddle)
  ) {
    
    calculate_reflection_and_set_velocity(); //ball_x_velocity = -ball_x_velocity;

  // If ball on left side!
  } else if (ball_x < (SCREEN_WIDTH_FLOAT / 2) &&
    // Checks if to the right of the paddle
    ball_x >= (paddle_x - paddle_width) && 
    // Checks if to the left of the paddle
    ball_x <= (paddle_x + paddle_width) && 
    
    // if ball y is within paddle height
    ball_y <= (paddle1_y + half_paddle) && 
    ball_y >= (paddle1_y - half_paddle)
  ){
    //ball_x_velocity = -ball_x_velocity;
    calculate_reflection_and_set_velocity();
  }
}

// Inverses the velocity on edge bounces.
void set_new_velocity_on_edge() {
  if (ball_x > SCREEN_WIDTH_FLOAT || ball_x < 0) {
    ball_x_velocity = -ball_x_velocity;

    // adds points to a player if they score (ball reaches the end of the screen)
    if (ball_x > SCREEN_WIDTH_FLOAT) {
      player1_points++;
    }
    else if (ball_x < 0) {
      player2_points++;
    }

    current_screen = SCORE; // goes to score if ball hits one of the edges
  }
  // If the ball is outside the screen y-wise it'll invert upwards velocity, e.g bounce.
  if (ball_y > SCREEN_HEIGHT_FLOAT || ball_y < 0){
    ball_y_velocity = -ball_y_velocity;
  }
}


// adds ball velocity to it's position
void update_ball_pos_on_velocity() {
  ball_x += ball_x_velocity;
  ball_y += ball_y_velocity;
}


// creates the ball and displays it based on it's position
void display_ball() {

  // changes velocity if it hits paddle or edge
  set_new_velocity_on_paddle_collision();
  set_new_velocity_on_edge();  // Sets the new velocity, important that it's called before get_between.
  
  // Makes sure the ball is within the screen.
  ball_x = get_between(ball_x, 0, SCREEN_WIDTH_FLOAT);
  ball_y = get_between(ball_y, 0, SCREEN_HEIGHT_FLOAT);

  // Creates 4 pixels for the ball based on the center point.
  set_pixel(floor_custom(ball_x), floor_custom(ball_y));
  set_pixel(floor_custom(ball_x), ceil_custom(ball_y, SCREEN_HEIGHT_FLOAT));
  set_pixel(ceil_custom(ball_x, SCREEN_WIDTH_FLOAT), ceil_custom(ball_y, SCREEN_HEIGHT_FLOAT));
  set_pixel(ceil_custom(ball_x, SCREEN_WIDTH_FLOAT), floor_custom(ball_y));
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


// updates the screen when moving pixles
void update_canvas() {
  clear_display();
  display_paddle();
  display_ball();
  display_image(display);
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////
  AI
*///////////////////////////////////////////////////////////////////////////////////////////////////

float ai_paddle_y_velocity = 0.5;

// initializes ai based on difficulty
void difficulty_init() {
  
  // easy
  if (difficulty == EASY) {
    ai_reaction_pixels = 10;
    ai_centers = 0;
    ai_paddle_y_velocity = 0.015; // DEBUG: Should be 0.15
  }

  // hard
  else if (difficulty == HARD) {
    ai_reaction_pixels == 100;
    ai_centers = 1;
    ai_paddle_y_velocity = 0.5;
  }

  // increasing difficulty
  else if (difficulty == INCREASING) {
    ai_reaction_pixels = 10;
    ai_paddle_y_velocity = 0.3;
  }

  game_time = 0; // restarts game time
}


// moves ai witihin it's corresponding reaction time and relation to ball
void ai_move() {

  // if distance to ball is within length 
  if ((SCREEN_WIDTH_FLOAT - paddle_x - ball_x ) < ai_reaction_pixels) {
    
    //if ball is coming towards AI paddle2
    if((ball_x_velocity > 0)) {

      // if enough delay has passed for gameplay
      if ((total_timeout - press_delay) > delay_value_game_inputs) {
      
        // if ball is lower than paddle
        if(ball_y > paddle2_y) {
          paddle2_y += ai_paddle_y_velocity;
        }

        // if ball is higher than paddle, lower it's height
        else if (ball_y < paddle2_y) {
          paddle2_y -= ai_paddle_y_velocity; 
        }
      }
    }
  }

  // if ai is allowed to center it does so when ball is not within reaction time
  else if (ai_centers) {
    
    // if not centered 
    if (paddle2_y != 15) {

      // over middle, decrease position
      if (paddle2_y > 15.5) {
        paddle2_y -= ai_paddle_y_velocity;
      }

      // under middle, increase position
      else if (paddle2_y < 15.5) {
        paddle2_y += ai_paddle_y_velocity;
      }
    }
  }
}


// updates AI inputs depending on where ball is, increase it's height 
void ai_update() {

  // easy or hard, move as usual
  if (difficulty == EASY || difficulty == HARD) {
    ai_move();
  }

  // increasing difficulty, adds reaction pixels can center over time
  else if (difficulty == INCREASING) {

    // increases reaction time with game time to a limit
    if (ai_reaction_pixels < 100) {
      ai_reaction_pixels += 0.01;
    }

    if (ai_paddle_y_velocity < 2) {
      ai_paddle_y_velocity *= 1.001;
    }

    // starts to center after a while
    if (difficulty == INCREASING && (game_time > 100)) {
      ai_centers = 1;
    }

    ball_x_velocity *= speedup;
    ball_y_velocity *= speedup;

    ai_move();
  }
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

  // reset points
  player1_points = 0;
  player2_points = 0;
}


// chose difficulty
void difficulty_options() {
  current_screen = DIFFICULTY; // in screen
  display_string(0, "1. Easy");
  display_string(1, "2. Hard");
  display_string(2, "3. Increasing");
  display_string(3, "4. Back");
  display_update();
}


// singleplayer screen
void singleplayer() {
  current_screen = SINGLEPLAYER; // in singleplayer
  is_singleplayer = 1;
  center_ball(); // ??
  difficulty_init(); // initialize the selected difficulty
}


// multiplayer screen
void multiplayer() {
  current_screen = MULTIPLAYER; // in multiplayer
  is_singleplayer = 0;
  center_ball(); // ??
}


// leaderboard screen
void leaderboard() {
  current_screen = LEADERBOARD; // in leaderboard

  display_string(0, name_and_points_array(leaderboard_names[0], leaderboard_scores[0]));
  display_string(1, name_and_points_array(leaderboard_names[1], leaderboard_scores[1]));
  display_string(2, name_and_points_array(leaderboard_names[2], leaderboard_scores[2]));
  display_string(3, "3. Back");
  display_update();
}


// player highscore name
int selected_char_position = 0;
char name[4];

// saves and sorts leaderboard
void save_to_leaderboard() {

  int i = ARRAY_SIZE - 1;
  for (i; i >= 0; i--) {

    // if player points larger than current leaderboard positions score
    if (player1_points > leaderboard_scores[i]) {
      
        // previous score 
        if (i+1 < ARRAY_SIZE)
          leaderboard_scores[i+1] = leaderboard_scores[i]; 

        // previous name char wise
        leaderboard_names[i+1][0] = leaderboard_names[i][0];
        leaderboard_names[i+1][1] = leaderboard_names[i][1];
        leaderboard_names[i+1][2] = leaderboard_names[i][2];
        leaderboard_names[i+1][3] = '\0';
    }

    // no need to check scores over if current is larger than players points, since leaderboard is sorted
    else {
      break;
    }
  }
  //current score 
  leaderboard_scores[i+1] = player1_points;

  // current name
  leaderboard_names[i+1][0] = name[0];
  leaderboard_names[i+1][1] = name[1];
  leaderboard_names[i+1][2] = name[2];
  leaderboard_names[i+1][3] = '\0';
}

// player can write name to leaderboard after win
void write_to_leaderboard() {
  current_screen = WRITE_LEADERBOARD;
  
  // if 3 chars, save
  if (selected_char_position == 3) {
    selected_char_position = 0; // resets char position for next highscore
    save_to_leaderboard(); // saves score
    current_screen = MENU; // goes back to menu
  }

  // keep inputing
  else {
    name[selected_char_position] = selected_char;

    display_string(0, name); // namn
    display_string(1, "2. Select"); // bokstav 
    display_string(2, "3. Right"); 
    display_string(3, "4. Left");
    display_update();
  }
}

// if a new highscore has been achieved
int got_highscore() {
  int highscore = 0;
  
  int i = 0;
  for (i = 0; i < 3; i++) {
    if (player1_points > leaderboard_scores[i])
      highscore = 1;
  }
  
  return highscore;
}


// show current score in between points
void score() {
  current_screen = SCORE;
  
  // if a player has won in multiplayer, go to results
  if ((!is_singleplayer && (player1_points >= rounds_to_win || player2_points >= rounds_to_win)) 
      || (is_singleplayer && player2_points > 0)) {
    current_screen = RESULTS;
  }

  // if no player has won
  else {

    // player1 points to char array
    char p1p[] = "P1 =  "; 
    p1p[5] = int_to_char(player1_points);

    // player2 name
    char p2p[] = "P2 =  "; 

    // clears player2 name if in singleplayer
    if (is_singleplayer) {
      int i = 0;
      for (i = 0; i <= 3; i++) {
        p2p[i] = ' ';
      }
    }

    // adds P2 if in multiplayer
    else {
      p2p[5] = int_to_char(player2_points);
    }
    
    // scoreboard
    display_string(0, p1p);
    display_string(1, p2p);
    display_string(2, "");
    display_string(3, "3. Continue");
    display_update();
  }

  // reset paddle and position
  center_ball();
  paddle1_y = 15.5f;
  paddle2_y = 15.5f;
}


// shows which player won after a game
void results () {
  current_screen = RESULTS;

  // from singleplayer
  if(is_singleplayer) {
    char points[] = "P =  ";
    points[5] = int_to_char(player1_points);

    display_string(1, points);

    // new highscore
    if(got_highscore()) {
      display_string(0, "NEW HIGHSCORE!");
      display_string(2, "3. Submit");

      // reset name
      name[0] = ' ';
      name[1] = ' ';
      name[2] = ' ';
    }

    // no new highscore
    else {
      display_string(0, "");
      display_string(2, "");
    }
  }

  // from multiplayer
  else{

    // player1 won game
    if (player1_points >= rounds_to_win) {
      display_string(0, "Player1 won!");
    }

    // player2 won game
    else if (player2_points >= rounds_to_win) {    
      display_string(0, "Player2 won!");
    }
    display_string(1, "");
  }

  // go back
  display_string(3, "4. Menu");
  display_update();
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
      current_screen = DIFFICULTY;
    }

    // difficulty options
    else if (current_screen == DIFFICULTY) {
      difficulty = EASY;
      current_screen = SINGLEPLAYER;
    }
  }

  // if enough delay has passed for gameplay
  if ((total_timeout - press_delay) > delay_value_game_inputs) {

    // multiplayer
    if (current_screen == MULTIPLAYER) {
      paddle2_y -= paddle_y_velocity;
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

    // difficulty options
    else if (current_screen == DIFFICULTY) {
      difficulty = HARD;
      current_screen = SINGLEPLAYER;
    }

    // play
    else if (current_screen == PLAY) {
      current_screen = MULTIPLAYER;
    }

    // write leaderboard
    else if (current_screen == WRITE_LEADERBOARD) {
      selected_char_position++; 
      write_to_leaderboard(); // regen
      
    }
  }

  // if enough delay has passed for gameplay input
  if (total_timeout - press_delay > delay_value_game_inputs) {

    // multiplayer
    if (current_screen == MULTIPLAYER) {
      paddle2_y += paddle_y_velocity;
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

    // difficulty options
    else if (current_screen == DIFFICULTY) {
      difficulty = INCREASING;
      current_screen = SINGLEPLAYER;
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
    else if (current_screen == RESULTS && got_highscore()) {
      current_screen = WRITE_LEADERBOARD; 
    }

    // write to leaderboard
    else if ((current_screen == WRITE_LEADERBOARD) && (selected_char < 'Z')) {
      selected_char++; // right
      write_to_leaderboard(); // regenerates leaderboard
    }
  }

  // if enough delay has passed for gameplay inputs
  if (total_timeout - press_delay > delay_value_game_inputs) {
    // game
    if (current_screen == SINGLEPLAYER || current_screen == MULTIPLAYER) {
      paddle1_y -= paddle_y_velocity;
    }
    
    press_delay = total_timeout; // reset optionsdelay to present totaltimeout
  }
}


// BTN4
void button4() {

  // if enough delay has passed for menu switching
  if((total_timeout - press_delay) > delay_value_menu_inputs) {

    // difficulty options
    if (current_screen == DIFFICULTY) {
      current_screen = PLAY;
    }

    // results
    else if (current_screen == RESULTS) {
      current_screen = PLAY;
    }

    // write to leaderboard
    else if ((current_screen == WRITE_LEADERBOARD) && (selected_char > 'A')) {
      selected_char--; // regenerates leaderboard
      write_to_leaderboard(); // regenerates leaderboard
    }
  }
  // if enough delay has passed
  if (total_timeout - press_delay > delay_value_game_inputs) {

    // game
    if (current_screen == SINGLEPLAYER || current_screen == MULTIPLAYER) {
      paddle1_y += paddle_y_velocity;
    }
    
    press_delay = total_timeout; // reset optionsdelay to present totaltimeout
  }
}


// SW1
void switch1() {
  current_screen = MENU; // secret quit game to menu wherever the user is
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

      case DIFFICULTY:
        difficulty_options();
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

      case WRITE_LEADERBOARD:
        write_to_leaderboard();
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

  // T2              111   
  T2CON = 0b1000000001000000; // timer on with PRE 16, see https://cdn.discordapp.com/attachments/876819453806010408/943170879490252810/unknown.png
  PR2 = 31250;  // (80M*10^6)/256/10

  // timing interrupts
  IEC(0) = IEC(0) | 0b100000000;
  IPC(2) = IPC(2) | 0b11100;  
  
  enable_interrupt(); // enable global interrupts

  menu(); // start with menu
}
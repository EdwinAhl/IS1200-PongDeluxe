#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int getsw() { return (PORTD >> 8) & 0b1111; }

int getbtns() { return (PORTD >> 4) & 0b1111; }
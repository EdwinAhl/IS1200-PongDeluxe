#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int getsw() { return (PORTD >> 8) & 0xF; } // switches start at bit 8, get value of all 4

int getbtns() { return ((PORTF >> 1) & 0b1) |  ((PORTD >> 4) & 0b1110); } // button 1 on PORTF bit 1, buttons 2-4 on PORTD start bit 4
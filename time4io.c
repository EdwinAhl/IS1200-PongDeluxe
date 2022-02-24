#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int getsw() { return (PORTD >> 8) & 0xF; } // switches start at bit 8, get value of all 4

int getbtns() { return ((PORTD >> 4) & 0b1110) | ((PORTF >> 1) & 0b1); } // buttons start at bit 4, get value of all 4
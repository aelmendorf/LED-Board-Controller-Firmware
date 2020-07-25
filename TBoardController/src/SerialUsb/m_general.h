#ifndef M_GENERAL_H_
#define M_GENERAL_H_

#define TRUE	1
#define FALSE	0

#define OFF		0
#define ON		1
#define TOGGLE	2
#define F_CPU 16000000UL

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>


// -----------------------------------------------------------------------------
// Bit manipulation and validation:
// -----------------------------------------------------------------------------

#define set(reg,bit)		reg |= (1<<(bit))
#define clear(reg,bit)		reg &= ~(1<<(bit))
#define toggle(reg,bit)		reg ^= (1<<(bit))
#define check(reg,bit)		(bool)(reg & (1<<(bit)))

// -----------------------------------------------------------------------------
// Disable JTAG to access F4-F7:
// -----------------------------------------------------------------------------
#define m_disableJTAG() MCUCR = (1 << JTD); MCUCR = (1 << JTD)

// -----------------------------------------------------------------------------
// Set the system clock:
// -----------------------------------------------------------------------------

#define m_clockdivide(val)	CLKPR = (1<<CLKPCE); CLKPR=val
// "val" must be an integer from 0 to 8
// this will divide the 16MHz system clock by 2^val:
// 0 = 16 MHz
// 1 = 8 MHz
// 2 = 4 MHz
// 3 = 2 MHz
// 4 = 1 MHz
// 5 = 500 kHz
// 6 = 250 kHz
// 7 = 125 kHz
// 8 = 62.5 kHz

#define m_wait(val) _delay_ms(val)
#define m_green(val) set(DDRE,2); if(val==OFF){set(PORTE,2);}else if(val==ON){clear(PORTE,2);}else if(val==TOGGLE){toggle(PORTE,2);}
#define m_red(val) set(DDRE,6); if(val==OFF){set(PORTE,6);}else if(val==ON){clear(PORTE,6);}else if(val==TOGGLE){toggle(PORTE,6);}
	
#endif /* M_GENERAL_H_ */
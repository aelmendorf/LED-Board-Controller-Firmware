/* 
* PWM.h
*
* Created: 9/7/2018
* Author: Andrew Elmendorf
* Company: Sensor Electronic Technology
* Description:
*  PWM Library
*
*+------------+---+--------+--------+--------+--------+--------+
*|            |   | 8b PS  | 8b PS  |   --   | 16b PS | 8b ePS |
*|            +---+--------+--------+--------+--------+--------+
*| ATmega32u4 | A |  D11   |   D9   |   --   |   D5   |  D13   |
*|            | B |   D3   |  D10   |   --   |   --   |  D10   |
*|            | C |   --   |  D11   |   --   |   --   |   --   |
*|            | D |   --   |   --   |   --   |   --   |   D6   |
*+------------+---+--------+--------+--------+--------+--------+
*/


#ifndef __PWM_H__
#define __PWM_H__
#define F_CPU 16000000UL
#include <avr/io.h>
#include <inttypes.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "../../src/General/CONFIG_TOOLS.h"
#include "../../src/General/MEM_HELPERS.h"

static void pwm_empty_interrupt(){}

class PWM
{
//variables
public:
protected:
	uint32_t base_clock;
	uint8_t PS_IDX[5]={0,0,0,0,0};
	uint16_t PS[5]={0,0,0,0,0};
private:

//functions
public:
	PWM():base_clock(F_CPU){}
	void set(const uint8_t &Timer,const char &ABCD_out,const uint32_t &Freq_HZ,const uint16_t dutyCycle_divisor=2,const bool invertOut=false);
	void start(const int8_t Timer=-1);
	void stop(const int8_t Timer=-1);
	
	void attachInterrupt(const uint8_t &Timer,const char &ABCD_out,void(*isr)());
	void detachInterrupt(const uint8_t &Timer,const char &ABCD_out);
	void set_register(const int8_t Timer = -1, const char ABCD_out = 'o', uint16_t register_value = 0);
	uint16_t get_register(const int8_t Timer = -1, const char ABCD_out = 'o');
	void enableInterrupt(const int8_t Timer = -1, const char ABCD_out = 'o');
	void disableInterrupt(const int8_t Timer = -1, const char ABCD_out = 'o');
	~PWM();
protected:
private:
	PWM( const PWM &c );
	PWM& operator=( const PWM &c );

}; //PWM

#endif //__PWM_H__

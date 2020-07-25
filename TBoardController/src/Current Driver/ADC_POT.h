/* 
* ADC_POT.h
*
* Created: 9/20/2018 3:07:18 PM
* Author: 600076
*/


#ifndef __ADC_POT_H__
#define __ADC_POT_H__
#include <TBoard.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <math.h>
#include <Helpers.h>
class ADC_POT
{
//variables
public:
protected:
private:
	volatile uint16_t adc_value;
	volatile uint8_t adc_flag;
	volatile uint8_t safe_value;
//functions
public:
	ADC_POT();
	~ADC_POT();
	void enable();
	void disable();
	uint8_t ready();
	uint16_t read_wait();
	void isr();
	uint16_t Value();
protected:
private:
	ADC_POT( const ADC_POT &c );
	ADC_POT& operator=( const ADC_POT &c );

}; //ADC_POT

#endif //__ADC_POT_H__

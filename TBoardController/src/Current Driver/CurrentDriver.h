/* 
* CurrentDriver.h
*
* Created: 9/19/2018 10:58:12 AM
* Author: 600076
*/


#ifndef __CURRENTDRIVER_H__
#define __CURRENTDRIVER_H__
#include <TBoard.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <math.h>
#include <Helpers.h>
#include <ADC_POT.h>


class CurrentDriver
{
//variables
public:
	uint16_t currentValue;
	uint16_t newValue;
	uint16_t adc_value;
	ADC_POT *adc_driver;
	uint16_t pwm_value;
	uint8_t display_index;
	uint16_t lookup[4][3]={
		{0,255,CURRENT_1},
		{256,511,CURRENT_2},
		{512,767,CURRENT_3},
		{768,1023,CURRENT_4},
	};
protected:
private:

//functions
public:
	CurrentDriver();
	CurrentDriver(ADC_POT *adc);
	~CurrentDriver();
	uint16_t read_input(void);
	uint16_t ADC_Value(void);
	void turn_off(void);
	void turn_on(void);
	void init(void);
	uint16_t CurrentValue(void);
	void set_pwm(volatile uint16_t* reg);
	void turn_on_manual(const uint8_t &value);
	void set_current(uint8_t reset);
	uint8_t get_display(void);
	int8_t set_current_value(uint8_t value);
	void enable_adc(void);
	void disable_adc(void);
protected:
private:
	void map_adc(void);
	//CurrentDriver( const CurrentDriver &c );
	//CurrentDriver& operator=( const CurrentDriver &c );

}; //CurrentDriver

#endif //__CURRENTDRIVER_H__

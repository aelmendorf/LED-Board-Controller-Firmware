/* 
* CurrentDriver.cpp
*
* Created: 9/19/2018 10:58:12 AM
* Author: 600076
*/


#include "CurrentDriver.h"

CurrentDriver::CurrentDriver(ADC_POT *adc){
	this->adc_driver=adc;
	this->currentValue=0;
	this->adc_value=0;
	this->pwm_value=0;
	this->display_index=0;
}

// default constructor
CurrentDriver::CurrentDriver()
{
} //CurrentDriver

// default destructor
CurrentDriver::~CurrentDriver()
{
} //~CurrentDriver

void CurrentDriver::init(){
	//Current driver.  set output then pull low
	DDRC|=(1<<CURRENT_DRIVER);
	PORTC&=~(1<<CURRENT_DRIVER);
}

uint16_t CurrentDriver::read_input(){
	this->adc_value=this->adc_driver->read_wait();
	this->map_adc();
	return this->currentValue;
}//End read_input

uint16_t CurrentDriver::ADC_Value(){
	return this->adc_value;
}

void CurrentDriver::enable_adc(){
	this->adc_driver->enable();
}

void CurrentDriver::disable_adc(){
	this->adc_driver->disable();
}

void CurrentDriver::set_current(uint8_t reset){
	if(this->currentValue<=0){
		PORTC&=~(1<<PORTC7);
		}else{
			int strobes=33-((8*this->currentValue)/5);
			if(reset){
				PORTC&=~(1<<PORTC7);
				_delay_us(5);
			}
			for(int i=0;i<strobes;i++){
				PORTC|=(1<<PORTC7);
				_delay_us(5);
				PORTC&=~(1<<PORTC7);
			}
			_delay_us(5);
			PORTC|=(1<<PORTC7);
			_delay_us(130);
	}//end if/else
}

void CurrentDriver::turn_off(){
	PORTC&=~(1<<PORTC7);
	_delay_us(5);
}

uint16_t CurrentDriver::CurrentValue(){
	return this->currentValue;
}

void CurrentDriver::turn_on(){
	this->set_current(0);
}

void CurrentDriver::turn_on_manual(const uint8_t &value){
	this->currentValue=value;
	this->set_current(1);
}

void CurrentDriver::map_adc(void){
	for(int i=0;i<4;i++){
		if(this->adc_value>=this->lookup[i][0] && this->adc_value<=this->lookup[i][1]){
			this->currentValue=this->lookup[i][2];
			this->pwm_value=MAX_ADC-(this->lookup[i][1]);
			this->display_index=i;
		}
	}
}//end map

int8_t CurrentDriver::set_current_value(uint8_t value){
	for(int i=0;i<4;i++){
		if(value==this->lookup[i][2]){
			this->currentValue=this->lookup[i][2];
			this->pwm_value=MAX_ADC-(this->lookup[i][1]);
			this->display_index=i;
			return i;
		}
	}
	return -1;
}

uint8_t CurrentDriver::get_display(void){
	return this->display_index;
}

void CurrentDriver::set_pwm(volatile uint16_t* reg){
	*reg=this->pwm_value;
}
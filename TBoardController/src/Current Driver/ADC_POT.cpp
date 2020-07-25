/* 
* ADC_POT.cpp
*
* Created: 9/20/2018 3:07:18 PM
* Author: 600076
*/


#include "ADC_POT.h"

// default constructor
ADC_POT::ADC_POT()
{
	this->adc_value=0;
	this->adc_flag=0;
	this->safe_value=0;
	ADMUX|=(1<<REFS0) |(1<<MUX0);
	//ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
} //ADC_POT

void ADC_POT::enable(){
	this->adc_value=0;
	this->adc_flag=0;
	this->safe_value=0;
	//ADCSRA|=(1<<ADIE);
	//ADCSRA|=(1<<ADSC);
}

void ADC_POT::disable(){
	this->adc_value=0;
	this->adc_flag=0;
	this->safe_value=0;			
	ADCSRA&=~(1<<ADIE);
}

uint8_t ADC_POT::ready(){
	uint8_t rdy=this->adc_flag;
	if(rdy){
		ADCSRA&=~(1<<ADIE);//be sure interrupt disabled
		this->adc_flag=0;
		this->safe_value=this->adc_value;
		ADCSRA|=(1<<ADIE);
		ADCSRA|=(1<<ADSC);
	}
	return rdy;
}

void ADC_POT::isr(){
	this->adc_value=MAX_ADC-ADC;
	OCR1B=this->adc_value;
	this->adc_flag=1;
	ADCSRA&=~(1<<ADIE);
}

uint16_t ADC_POT::Value(){
	return this->adc_value;
}

uint16_t ADC_POT::read_wait(){
	ADCSRA|=(1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	this->adc_value=ADC;
	return this->adc_value;
}

// default destructor
ADC_POT::~ADC_POT()
{
} //~ADC_POT

/*
* PWM.h
*
* Created: 9/7/2018
* Author: Andrew Elmendorf
* Company:
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


#include "PWM.h"

void(*pwm_interrupt0)()=*pwm_empty_interrupt;
void(*pwm_interrupt0a)()=*pwm_empty_interrupt;
void(*pwm_interrupt0b)()=*pwm_empty_interrupt;


void(*pwm_interrupt1)()=*pwm_empty_interrupt;
void(*pwm_interrupt1a)()=*pwm_empty_interrupt;
void(*pwm_interrupt1b)()=*pwm_empty_interrupt;
void(*pwm_interrupt1c)()=*pwm_empty_interrupt;

void(*pwm_interrupt3)()=*pwm_empty_interrupt;
void(*pwm_interrupt3a)()=*pwm_empty_interrupt;
void(*pwm_interrupt3b)()=*pwm_empty_interrupt;
void(*pwm_interrupt3c)()=*pwm_empty_interrupt;

void(*pwm_interrupt4)()=*pwm_empty_interrupt;
void(*pwm_interrupt4a)()=*pwm_empty_interrupt;
void(*pwm_interrupt4b)()=*pwm_empty_interrupt;
void(*pwm_interrupt4d)()=*pwm_empty_interrupt;

ISR(TIMER0_OVF_vect){pwm_interrupt0();}
ISR(TIMER0_COMPA_vect){pwm_interrupt0a();}
ISR(TIMER0_COMPB_vect){pwm_interrupt0b();}

ISR(TIMER1_OVF_vect){pwm_interrupt1();}
ISR(TIMER1_COMPA_vect){pwm_interrupt1a();}
ISR(TIMER1_COMPB_vect){pwm_interrupt1b();}
ISR(TIMER1_COMPC_vect){pwm_interrupt1c();}
	
ISR(TIMER3_OVF_vect){pwm_interrupt3();}
ISR(TIMER3_COMPA_vect){pwm_interrupt3a();}
ISR(TIMER3_COMPB_vect){pwm_interrupt3b();}
ISR(TIMER3_COMPC_vect){pwm_interrupt3c();}

ISR(TIMER4_OVF_vect){pwm_interrupt4();}
ISR(TIMER4_COMPA_vect){pwm_interrupt4a();}
ISR(TIMER4_COMPB_vect){pwm_interrupt4b();}
ISR(TIMER4_COMPD_vect){pwm_interrupt4d();}

#define OCR0A_pin PINB7
#define OCR0B_pin PIND0
#define OCR1A_pin PINB5
#define OCR1B_pin PINB6
#define OCR1C_pin PINB7
#define OCR3A_pin PINC6
#define OCR4A_pin PINC6
#define OCR4B_pin PINB6
#define OCR4D_pin PIND7


void PWM::set(const uint8_t &Timer,const char &ABCD_out,const uint32_t &Freq_HZ,const uint16_t dutyCycle_divisor/* =2 */,const bool invertOut/* =false */)
{
	
	uint16_t CSx3210=0;
	uint32_t freq_count=base_clock/Freq_HZ;
	uint32_t period_reg=0;
	uint32_t pulse_width_reg=0;
	//Prescalers 
	//                   0 1 2 3 4  5  6  7  8   9   10  11   12   13   14    15
	uint16_t _PS[16]={ 0,1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384 };
	// Index of the regular set of prescalar values
	uint8_t N[6] = {0,1,4,7,9,11}; // _PS = { 0,1,8,64,256,1024 };
	
	const uint8_t COMx10=2+invertOut;
	
	switch(Timer){
		case 0:{
			//8bit timer regular prescaler
			while ((freq_count > (1 + _PS[N[CSx3210]] * 0xFF)) & (CSx3210 < 5)) {
				 ++CSx3210; 
			}
			this->PS[Timer] = _PS[N[CSx3210]];
			break;
		}
		case 1:{
			//16bit timer regular prescaler
			while ((freq_count > (1 + _PS[N[CSx3210]] * 0xFFFF)) & (CSx3210 < 5)) {
				++CSx3210;
			}
			this->PS[Timer] = _PS[N[CSx3210]];
			break;
		}
		case 3:{
			//16bit timer regular prescaler
			while ((freq_count > (1 + _PS[N[CSx3210]] * 0xFFFF)) & (CSx3210 < 5)) {
				++CSx3210;
			}
			this->PS[Timer] = _PS[N[CSx3210]];
			break;
		}
		case 4:{
			//8bit timer extended prescaler
			while ((freq_count > (1 + _PS[N[CSx3210]] * 0xFF)) & (CSx3210 < 15)) {
				++CSx3210;
			}
			this->PS[Timer] = _PS[N[CSx3210]];
		
		}
	}//End switch prescaler by timer
	this->PS_IDX[Timer]=CSx3210;
	period_reg=(freq_count/this->PS[Timer])-1;
	pulse_width_reg=period_reg/dutyCycle_divisor;
	
	// WGMx[210]  =  [111] Fast PWM, OCRxA as top
	// WGMx[3210] = [1110] Fast PWM,  ICRx as top
	// WGMx[3210] = [1111] Fast PWM, OCRxA as top (Timer4 - OCR4C)
	switch(Timer){
		case 0:{
			OCR0A = period_reg;
			// set the COMx pin mode
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					portPinMode(&DDRB,&PORTB,OCR0A_pin,OUTPUT,HIGH,false);
					//TCCR0A = [COM0A1|COM0A0|COM0B1|COM0B0|   -  |   -  | WGM01| WGM00]
					// clear the old bits
					TCCR0A &= ~(1<<COM0A1) & ~(1<<COM0A0);
					// set the new bits
					TCCR0A |= (1<<COM0A0); // NB: OCR0A is toggled, hence half the frequency (its the only option)
					break;
				default:
					OCR0B = period_reg;
					portPinMode(&DDRD,&PORTD,OCR0B_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR0A &= ~(1<<COM0B1) & ~(1<<COM0B0);
					// set the new bits
					TCCR0A |= (COMx10 << 4);
					break;
			}
			
			// Set the waveform mode to fast PWM
			//TCCR0A = [COM0A1|COM0A0|COM0B1|COM0B0|   -  |   -  | WGM01| WGM00]
			//TCCR0B = [FOC0A | FOC0A|   -  |   -  | WGM02|  CS02|  CS01|  CS00]
			// clear the old bits
			TCCR0A &= ~(1<<WGM01) & ~(1<<WGM00);
			TCCR0B &= ~(1<<WGM02);
			// set the new bits
			TCCR0A |= (1<<WGM01) | (1<<WGM00);
			TCCR0B |= (1<<WGM02);
			break;
		}
		case 1:{
			ICR1 = period_reg;
			// set the COMx pin mode
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					OCR1A = pulse_width_reg;
					portPinMode(&DDRB,&PORTB,OCR1A_pin,OUTPUT,HIGH,false);
					//TCCR1A = [COM1A1|COM1A0|COM1B1|COM1B0|   -  |   -  | WGM11| WGM10]
					// clear the old bits
					TCCR1A &= ~(1<<COM1A1) & ~(1<<COM1A0);
					// set the new bits
					TCCR1A |= (COMx10 << 6);
					break;
				case 'b':
				case 'B':
					OCR1B = pulse_width_reg;
					portPinMode(&DDRB,&PORTB,OCR1B_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR1A &= ~(1<<COM1B1) & ~(1<<COM1B0);
					// set the new bits
					TCCR1A |= (COMx10 << 4);
					break;
			}
			// Set the waveform mode to fast PWM, ICR1 as TOP : 1110
			//TCCR1A = [COM1A1|COM1A0|COM1B1|COM1B0|   -  |   -  | WGM11| WGM10]
			//TCCR1B = [ ICNC1| ICES1|   -  | WGM13| WGM12|  CS12|  CS11|  CS10]
			// clear the old bits
			TCCR1A &= ~(1<<WGM11) & ~(1<<WGM10);
			TCCR1B &= ~(1<<WGM13) & ~(1<<WGM12);
			// set the new bits
			TCCR1A |= (1<<WGM11);
			TCCR1B |= 1<<(WGM13) | (1<<WGM12);
			break;
		}
		case 3:{
			// set the period register
			ICR3 = period_reg;

			//TCCR3A = [COM3A1|COM3A0|COM3B1|COM3B0|COM3C1|COM3C0| WGM31| WGM30]
			//TCCR3B = [ ICNC3| ICES3|   -  | WGM33| WGM32|  CS32|  CS31|  CS30]
			//TCCR3C = [ FOC3A|   -  |   -  |   -  |   -  |   -  |   -  |   -  ]
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					OCR3A = pulse_width_reg;
					portPinMode(&DDRC,&PORTC,OCR3A_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR3A &= ~(1<<COM3A1) & ~(1<<COM3A0);
					// set the new bits
					TCCR3A = (COMx10 << 6);
					break;
				case 'b':
				case 'B':
					OCR3B = pulse_width_reg;
					portPinMode(&DDRC,&PORTC,OCR3A_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR3A &= ~(1<<COM3B1) & ~(1<<COM3B0);
					// set the new bits
					TCCR3A = (COMx10 << 4);
					break;
				case 'c':
				case 'C':
					OCR3C = pulse_width_reg;
					portPinMode(&DDRC,&PORTC,OCR3A_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR3A &= ~(1<<COM3C1) & ~(1<<COM3C0);
					// set the new bits
					TCCR3A = (COMx10 << 2);
					break;
			}
			// Set the waveform mode to fast PWM, ICR3 as TOP : 1110
			//TCCR3A = [COM3A1|COM3A0|COM3B1|COM3B0|COM3C1|COM3C0| WGM31| WGM30]
			//TCCR3B = [ ICNC3| ICES3|   -  | WGM33| WGM32|  CS32|  CS31|  CS30]
			// clear the old bits
			TCCR3A &= ~(1<<WGM31) & ~(1<<WGM30);
			TCCR3B &= ~(1<<WGM33) & ~(1<<WGM32);
			// set the new bits
			TCCR3A |= (1<<WGM31);
			TCCR3B |= (1<<WGM33) | (1<<WGM32);					
			break;
		}
		case 4:{
			OCR4C = period_reg;
		//TCCR4A = [ COM4A1| COM4A0| COM4B1| COM4B0| FOC4A| FOC4B| PWM4A| PWM4B]
		//TCCR4B = [  PWM4X|   PSR4| DTPS41| DTPS40|  CS43|  CS42|  CS41|  CS40]
		//TCCR4C = [COM4A1S|COM4A0S|COM4B1S|COM4B0S|COM4D1|COM4D0| FOC4D| PWM4D]
		//TCCR4D = [  FPIE4|  FPEN4|  FPNC4|  FPES4| FPAC4|  FPF4| WGM41| WGM40]
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					OCR4A = pulse_width_reg;

					portPinMode(&DDRC,&PORTC,OCR4A_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR4A &= ~(1<<COM4A1) & ~(1<<COM4A0) & ~(1<<PWM4A);
					// set the new bits
					TCCR4A |= (COMx10 << 6) | (1<<PWM4A);
					break;
				case 'b':
				case 'B':
					OCR4B = pulse_width_reg;
					portPinMode(&DDRB,&PORTB,OCR4B_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR4A &= ~(1<<COM4B1) & ~(1<<COM4B0) & ~(1<<PWM4B);
					// set the new bits
					TCCR4A |= (COMx10 << 4) | (PWM4B);
					break;
				case 'd':
				case 'D':
					OCR4D = pulse_width_reg;
					portPinMode(&DDRD,&PORTD,OCR4D_pin,OUTPUT,HIGH,false);
					// clear the old bits
					TCCR4C &= ~(1<<COM4D1) & ~(1<<COM4D0) & ~(1<<PWM4D);
					// set the new bits
					TCCR4C |= (COMx10 << 2) | (1<<PWM4D);
					break;
			}
			//WGM4[10]  =  [00] Fast PWM,  OCR4C as top
			TCCR4D &= ~(1<<WGM41) & ~(1<<WGM40);
			break;
		}
	}
}
void PWM::start(const int8_t Timer)
{
	// stop the Timer before setting the new prescalar value
	this->stop(Timer);

	this->enableInterrupt(Timer);

	// Set the PWM prescalar (starts the timer)
	switch (Timer)
	{
		case 0:
			//TCCR0B = [ FOC0A| FOC0A|   -  |   -  | WGM02|  CS02|  CS01|  CS00]
			TCCR0B |= PS_IDX[0];
			break;
		case 1:
			//TCCR1B = [ ICNC1| ICES1|   -  | WGM13| WGM12|  CS12|  CS11|  CS10]
			TCCR1B |= PS_IDX[1];
			break;
		case 3:
			//TCCR3B = [ ICNC3| ICES3|   -  | WGM33| WGM32|  CS32|  CS31|  CS30]
			TCCR3B |= PS_IDX[3];
			break;
		case 4:
			//TCCR4B = [  PWM4X|   PSR4| DTPS41| DTPS40|  CS43|  CS42|  CS41|  CS40]
			TCCR4B |= PS_IDX[4];
		break;
			case -1: // All
			TCCR0B |= PS_IDX[0];
			TCCR1B |= PS_IDX[1];
			TCCR3B |= PS_IDX[3];
			TCCR4B |= PS_IDX[4];
			break;
	}
}
void PWM::stop(const int8_t Timer)
{
	// Set the PWM prescalar to zero (stops the timer)
	switch (Timer)
	{
		case 0:
			//TCCR0B = [ FOC0A| FOC0A|   -  |   -  | WGM02|  CS02|  CS01|  CS00]
			TCCR0B &= ~(1<<CS02) & ~(1<<CS01) & ~(1<<CS00);
		break;
		case 1:
			//TCCR1B = [ ICNC1| ICES1|   -  | WGM13| WGM12|  CS12|  CS11|  CS10]
			TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10);
		break;
		case 3:
			//TCCR3B = [ ICNC3| ICES3|   -  | WGM33| WGM32|  CS32|  CS31|  CS30]
			TCCR3B &= ~(1<<CS32) & ~(1<<CS31) & ~(1<<CS30);
		break;
		case 4:
			//TCCR4B = [ PWM4X|  PSR4|DTPS41|DTPS40|  CS43|  CS42|  CS41|  CS40]
			TCCR4B &= ~(1<<CS43) & ~(1<<CS42) & ~(1<<CS41) & ~(1<<CS40);
		break;
		case -1: // All
			TCCR0B &= ~(1<<CS02) & ~(1<<CS01) & ~(1<<CS00);
			TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10);
			TCCR3B &= ~(1<<CS32) & ~(1<<CS31) & ~(1<<CS30);
			TCCR4B &= ~(1<<CS43) & ~(1<<CS42) & ~(1<<CS41) & ~(1<<CS40);
			break;
	}
	this->disableInterrupt(Timer);
}
void PWM::attachInterrupt(const uint8_t &Timer,const char &ABCD_out,void(*isr)()){
	this->disableInterrupt(Timer,ABCD_out);
	switch (Timer)
	{
		case 1:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					pwm_interrupt1a = isr;
					break;
				case 'b':
				case 'B':
					pwm_interrupt1b = isr;
					break;
				default:
					pwm_interrupt1 = isr;
			}
			break;
		case 3:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					pwm_interrupt3a = isr;
					break;
				case 'b':
				case 'B':
					pwm_interrupt3b = isr;
					break;
				case 'c':
				case 'C':
					pwm_interrupt3c = isr;
					break;
				default:
					pwm_interrupt3 = isr;
			}
			break;
		case 4:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					pwm_interrupt4a = isr;
					break;
				case 'b':
				case 'B':
					pwm_interrupt4b = isr;
					break;
				case 'd':
				case 'D':
					pwm_interrupt4d = isr;
					break;
				default:
					pwm_interrupt4 = isr;
			}
			break;
	}
}
void PWM::detachInterrupt(const uint8_t &Timer,const char &ABCD_out){
	switch (Timer)
	{
		case 1:
		switch (ABCD_out)
		{
			case 'a':
			case 'A':
			pwm_interrupt1a = pwm_interrupt1a;
			break;
			case 'b':
			case 'B':
			pwm_interrupt1b = pwm_interrupt1b;
			break;
			default:
			pwm_interrupt1 = pwm_interrupt1;
		}
		break;
		case 3:
		switch (ABCD_out)
		{
			case 'a':
			case 'A':
			pwm_interrupt3a = pwm_interrupt3a;
			break;
			case 'b':
			case 'B':
			pwm_interrupt3b = pwm_interrupt3b;
			break;
			case 'c':
			case 'C':
			pwm_interrupt3c = pwm_interrupt3c;
			break;
			default:
			pwm_interrupt3 = pwm_interrupt3;
		}
		break;
		case 4:
		switch (ABCD_out)
		{
			case 'a':
			case 'A':
			pwm_interrupt4a = pwm_interrupt4a;
			break;
			case 'b':
			case 'B':
			pwm_interrupt4b = pwm_interrupt4b;
			break;
			case 'd':
			case 'D':
			pwm_interrupt4d = pwm_interrupt4d;
			break;
			default:
			pwm_interrupt4 = pwm_interrupt4;
		}
		break;
	}	
}
void PWM::enableInterrupt(const int8_t Timer , const char ABCD_out)
{
	//Timer interrupts
	//TIMSK0 = [   -  |   -  |   -  |   -  |   -  |OCIE0B|OCIE0A| TOIE0]
	//TIMSK1 = [   -  |   -  | ICIE1|   -  |OCIE1C|OCIE1B|OCIE1A| TOIE1]
	//TIMSK3 = [   -  |   -  | ICIE3|   -  |OCIE3C|OCIE3B|OCIE3A| TOIE3]
	//TIMSK4 = [OCIE4D|OCIE4A|OCIE4B|   -  |   -  | TOIE4|   -  |   -  ]
		
	switch (Timer)
	{
		case 0:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK0 |= (1<<OCIE0A); break;
				case 'b':
				case 'B':
					TIMSK0 |= (1<<OCIE0B); break;
				default:
					TIMSK0 |= (1<<TOIE0);
			}
			break;
		case 1:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK1 |= (1<<OCIE1A); break;
				case 'b':
				case 'B':
					TIMSK1 |= (1<<OCIE1B); break;
				case 'c':
				case 'C':
					TIMSK1 |= (1<<OCIE1C); break;
				default:
					TIMSK1 |= (1<<TOIE1);
			}
			break;
		case 3:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK3 |= (1<<OCIE3A); break;
				case 'b':
				case 'B':
					TIMSK3 |= (1<<OCIE3B); break;
				case 'c':
				case 'C':
					TIMSK3 |= (1<<OCIE3C); break;
				default:
					TIMSK3 |= (1<<TOIE3);
			}
			break;
		case 4:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK4 |= (1<<OCIE4A); break;
				case 'b':
				case 'B':
					TIMSK4 |= (1<<OCIE4B); break;
				case 'd':
				case 'D':
					TIMSK4 |= (1<<OCIE4D); break;
				default:
					TIMSK4 |= (1<<TOIE4);
			}
			break;
	}
}
void PWM::disableInterrupt(const int8_t Timer, const char ABCD_out)
{
	// Timer interrupts
	//TIMSK0 = [   -  |   -  |   -  |   -  |   -  |OCIE0B|OCIE0A| TOIE0]
	//TIMSK1 = [   -  |   -  | ICIE1|   -  |OCIE1C|OCIE1B|OCIE1A| TOIE1]
	//TIMSK3 = [   -  |   -  | ICIE3|   -  |OCIE3C|OCIE3B|OCIE3A| TOIE3]
	//TIMSK4 = [OCIE4D|OCIE4A|OCIE4B|   -  |   -  | TOIE4|   -  |   -  ]
	switch (Timer)
	{
		case 0:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK0 &= ~(1<<OCIE0A); break;
				case 'b':
				case 'B':
					TIMSK0 &= ~(1<<OCIE0B); break;
				default:
					TIMSK0 &= ~(1<<TOIE0);
			}
		break;
		case 1:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK1 &= ~(1<<OCIE1A); break;
				case 'b':
				case 'B':
					TIMSK1 &= ~(1<<OCIE1B); break;
				case 'c':
				case 'C':
					TIMSK1 &= ~(1<<OCIE1C); break;
				default:
					TIMSK1 &= ~(1<<TOIE1);
			}
		break;
		case 3:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK3 &= ~(1<<OCIE3A); break;
				case 'b':
				case 'B':
					TIMSK3 &= ~(1<<OCIE3B); break;
				case 'c':
				case 'C':
					TIMSK3 &= ~(1<<OCIE3C); break;
				default:
					TIMSK3 &= ~(1<<TOIE3);
			}
			break;
		case 4:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					TIMSK4 &= ~(1<<OCIE4A); break;
				case 'b':
				case 'B':
					TIMSK4 &= ~(1<<OCIE4B); break;
				case 'd':
				case 'D':
					TIMSK4 &= ~(1<<OCIE4D); break;
				default:
					TIMSK4 &= ~(1<<TOIE4);
			}
			break;
	}
}
uint16_t PWM::get_register(const int8_t Timer, const char ABCD_out)
{
	switch (Timer)
	{
		case 0:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
				return 0;
				case 'b':
				case 'B':
					return OCR0B;
				default:
					return OCR0A;
			}
		case 1:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
				return OCR1A;
				case 'b':
				case 'B':
				return OCR1B;
				case 'c':
				case 'C':
					return OCR1C;
				default:
					return ICR1;
			}
		case 3:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					return OCR3A;
				case 'b':
				case 'B':
					return OCR3B;
				case 'c':
				case 'C':
					return OCR3C;
				default:
					return ICR3;
			}
		case 4:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					return OCR4A;
				case 'b':
				case 'B':
					return OCR4B;
				case 'd':
				case 'D':
					return OCR4D;
				default:
					return OCR4C;
			}
		default:
			return 0;
	}
}
void PWM::set_register(const int8_t Timer, const char ABCD_out, uint16_t register_value)
{
	switch (Timer)
	{
		case 0:
			switch (ABCD_out)
			{
				case 'b':
				case 'B':
					OCR0B = register_value;
					break;
				case 'a':
				case 'A':
					default:
					OCR0A = register_value;
				break;
			}
		case 1:
			switch (ABCD_out)
			{
				case 'a':
				case 'A':
					OCR1A = register_value;
					break;
				case 'b':
				case 'B':
					OCR1B = register_value;
					break;
				case 'c':
				case 'C':
					OCR1C = register_value;
					break;
				default:
					ICR1 = register_value;
					break;
			}
		case 3:
		switch (ABCD_out)
		{
			case 'a':
			case 'A':
				OCR3A = register_value;
				break;
			case 'b':
			case 'B':
				OCR3B = register_value;
				break;
			case 'c':
			case 'C':
				OCR3C = register_value;
				break;
			default:
				ICR3 = register_value;
				break;
		}
		case 4:
		switch (ABCD_out)
		{
			case 'a':
			case 'A':
				OCR4A = register_value;
				break;
			case 'b':
			case 'B':
				OCR4B = register_value;
				break;
			case 'd':
			case 'D':
				OCR4D = register_value;
				break;
			default:
				OCR4C = register_value;
				break;
		}
	}
}

PWM::~PWM()
{
	//nothing to clean up
} //~PWM

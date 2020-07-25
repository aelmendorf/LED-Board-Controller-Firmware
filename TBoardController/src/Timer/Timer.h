
#ifndef __TIMER_H__
#define __TIMER_H__
#include <TBoard.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <Helpers.h>
#include <MilliTimer.h>

typedef struct TData{
	volatile uint8_t triggered:1;
	volatile uint8_t active:1;
	volatile uint32_t ms_delay;
	volatile uint32_t start_milli;
	volatile uint32_t elapsed_milli;
	TData(const uint32_t &ms){
		this->triggered=0;
		this->active=0;
		this->ms_delay=ms;
		this->start_milli=0;
		this->elapsed_milli=0;
	}
	uint8_t is_active(){return active;}
	uint8_t is_triggered(){return triggered;}
		
	inline TData& operator=(const TData &rhs){
		this->triggered=rhs.triggered;
		this->active=rhs.active;
		this->ms_delay=rhs.ms_delay;
		this->start_milli=rhs.start_milli;
		this->elapsed_milli=rhs.elapsed_milli;
		return *this;
	}	
}TData;


class Timer
{
//variables
public:
protected:
private:
	TData *timers;
	MilliTimer *milli_timer;
	uint8_t timer_count;
	uint8_t timer_allocated;
//functions
public:
	Timer(MilliTimer *tmr){
		this->milli_timer=tmr;
		this->timer_count=0;
		this->timer_allocated=0;
		this->add_timer(0);//now have 1 timer and 2 free spots
	}
	Timer();
	~Timer();
	void start(uint8_t tmr);
	void stop(uint8_t tmr);
	uint32_t elapsed(uint8_t tmr);
	uint8_t is_triggered(uint8_t tmr);
	void reset(uint8_t tmr);
	void set_delay(uint8_t tmr,const uint32_t &ms);
	int add_timer(const uint32_t &ms);
	
protected:
private:

}; //Timer

#endif //__TIMER_H__

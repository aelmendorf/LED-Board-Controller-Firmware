/* 
* Timer.cpp
*
* Created: 9/19/2018 9:08:52 AM
* Author: 600076
*/
#include "Timer.h"

/************************************************************************/
/* Timer Implementation                                                 */
/************************************************************************/
// default constructor

Timer::Timer()
{
	this->timer_count=0;
	this->timer_allocated=0;
	this->add_timer(0);//now have 1 timer and 2 free spots
} //Timer

void Timer::start(uint8_t tmr){
	if(tmr<=this->timer_count){
		this->timers[tmr].start_milli=this->milli_timer->Millis();
		this->timers[tmr].elapsed_milli=0;
		this->timers[tmr].triggered=0;
	}
}

uint32_t Timer::elapsed(uint8_t tmr){
	uint32_t e=0;
	if(tmr<=this->timer_count){
		e=this->timers[tmr].elapsed_milli;		
	}
	return e;
}

uint8_t Timer::is_triggered(uint8_t tmr){
	uint8_t trg=0;
	if(tmr<=this->timer_count){
		TData tmp=this->timers[tmr];
		tmp.elapsed_milli=this->milli_timer->Millis()-tmp.start_milli;
		tmp.triggered=tmp.elapsed_milli>=tmp.ms_delay;
		trg=tmp.triggered;
		this->timers[tmr]=tmp;
	}
	return trg;
}

void Timer::set_delay(uint8_t tmr,const uint32_t &ms){
	if(tmr<=this->timer_count){
		this->timers[tmr].ms_delay=ms;
	}
}

void Timer::reset(uint8_t tmr){
	if(tmr<=this->timer_count){
		this->timers[tmr].start_milli=this->milli_timer->Millis();
		this->timers[tmr].elapsed_milli=0;
		this->timers[tmr].triggered=0;
	}
}

int Timer::add_timer(const uint32_t &ms){
	TData tmr(ms);
	if(this->timer_count==this->timer_allocated){
		if(this->timer_allocated==0){
			this->timer_allocated=3;//allocate 3
		}else{
			this->timer_allocated*=2;//double array size.  realloc is expensive!!!!
		}//End check for resize amount
		
		void *tmp=realloc(this->timers,(this->timer_allocated*sizeof(TData)));
		if(!tmp){
			//bad things... malloc and resize failed!!!
			return -1;
		}
		this->timers=(TData*)tmp;
	}
	this->timers[this->timer_count]=tmr;
	this->timer_count++;
	return this->timer_count-1;
}

// default destructor
Timer::~Timer()
{
	if(this->timers!=0){
		free(this->timers);
	}
} //~Timer
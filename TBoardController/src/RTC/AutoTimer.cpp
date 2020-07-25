/* 
* AutoTimer.cpp
*
* Created: 10/5/2018 10:21:58
* Author: 600076
*/


#include "AutoTimer.h"

// default constructor
AutoTimer::AutoTimer()
{
	this->timer_count=0;
	this->timer_allocated=0;
	this->running_timer=-1;
	this->ready_timer=-1;
} //AutoTimer

// default destructor
AutoTimer::~AutoTimer()
{
	if(this->timers!=0){
		free(this->timers);
	}
} //~AutoTimer

int8_t AutoTimer::add_time(struct tm *now,uint8_t hr,uint8_t min,uint8_t sec,uint32_t duration_secs){
	AutoTime time;
	time.set(now,hr,min,sec,duration_secs);
	if(this->timer_count==this->timer_allocated){
		if(this->timer_allocated==0){
			this->timer_allocated=3;
		}else{
			this->timer_allocated*=2;
		}
		void *tmp=realloc(this->timers,(this->timer_allocated*sizeof(AutoTime)));
		if(!tmp){
			return -1;
		}
		this->timers=(AutoTime*)tmp;
	}
	this->timers[this->timer_count]=time;
	this->timer_count++;
	return this->timer_count-1;
}


int8_t AutoTimer::check_starts(time_t now_t){
	if(this->running_timer!=-1){
		return -1;
	}
	for(int i=0;i<timer_count;i++){
		if(this->timers[i].check_start(now_t)){
			this->ready_timer=i;
			return i;
		}
	}
	this->ready_timer=-1;
	return -1;
}

uint8_t AutoTimer::is_done(time_t now_t){
	return this->timers[this->running_timer].is_done(now_t);
}

int8_t AutoTimer::start_timer(time_t now_t){
	if(this->ready_timer!=-1){
		this->timers[this->ready_timer].start_timer(now_t);
		this->running_timer=this->ready_timer;
		for(int i=0;i<this->timer_count;i++){
			if(i!=this->running_timer){
				if(this->timers[this->running_timer].stop_t >= this->timers[i].start_t){
					int32_t shift=(this->timers[this->running_timer].stop_t-this->timers[i].start_t)+60;
					this->timers[i].shift_start(shift);
				}
			}
		}
		return TRUE;
	}//else no timer found to start
	return FALSE;
}

int8_t AutoTimer::stop_timer(uint8_t set_forward){
	if(this->running_timer!=-1){
		this->timers[this->running_timer].stop_timer(set_forward);
		this->running_timer=-1;
		this->ready_timer=-1;
		return TRUE;
	}
	return FALSE;
}

void AutoTimer::reset_timer(uint8_t timer,struct tm *now,uint8_t hr,uint8_t min,uint8_t sec,uint32_t duration_secs){
	this->timers[timer].set(now,hr,min,sec,duration_secs);
}

uint8_t AutoTimer::is_running_timer(){
	return this->running_timer!=-1;
}//end



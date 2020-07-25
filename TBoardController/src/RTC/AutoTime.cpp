/* 
* AutoTime.cpp
*
* Created: 10/5/2018 10:21:49
* Author: 600076
*/


#include "AutoTime.h"

// default constructor
AutoTime::AutoTime()
{
	this->duration=0;
	this->elapsed=0;
	this->started=0;
	this->done=0;
	this->start_delta=0;
	this->shifted=0;
} //AutoTime

// default destructor
AutoTime::~AutoTime()
{
	
} //~AutoTime


// void AutoTime::set(struct tm *now,uint8_t hr,uint8_t min,uint8_t sec,uint32_t duration_secs){
// 	this->start.tm_sec=sec;
// 	this->start.tm_min=min;
// 	this->start.tm_hour=hr;
// 
// 	this->start.tm_mon=now->tm_mon;
// 	this->start.tm_year=now->tm_year;
// 	this->start_t=mk_gmtime(&this->start);
// 	this->stop_t=this->start_t+duration_secs;
// 	gmtime_r(&this->stop_t,&this->stop);
// 	this->duration=duration_secs;
// 	
// 	if(now->tm_hour>(this->stop.tm_hour)){
// 		this->start.tm_mday=now->tm_mday+1;
// 		this->start_t=mk_gmtime(&this->start);
// 		this->stop_t=this->start_t+duration_secs;
// 		gmtime_r(&this->stop_t,&this->stop);
// 	}else{
// 		this->start.tm_mday=now->tm_mday;
// 	}
// 	this->elapsed=0;
// 	this->started=0;
// 	this->done=0;
// 	this->start_delta=0;
// }


void AutoTime::set(struct tm *now,uint8_t hr,uint8_t min,uint8_t sec,uint32_t duration_secs){
	
	this->start.tm_sec=sec;
	this->start.tm_min=min;
	this->start.tm_hour=hr;
	this->start.tm_mday=now->tm_mday;
	this->start.tm_mon=now->tm_mon;
	this->start.tm_year=now->tm_year;
	this->start_t=mk_gmtime(&this->start);
	this->stop_t=this->start_t+duration_secs;
	time_t now_t=mk_gmtime(now);
	if(now_t>this->stop_t){
		this->start_t+=ONE_DAY;
		gmtime_r(&this->start_t,&this->start);
		this->stop_t=(this->start_t+duration_secs);
	}
	gmtime_r(&this->stop_t,&this->stop);
	this->duration=duration_secs;
	this->elapsed=0;
	this->started=0;
	this->done=0;
	this->start_delta=0;
}


uint8_t AutoTime::check_start(time_t now_t){	
	if(this->start_t<=now_t && now_t<this->stop_t && !this->is_running()){
		return TRUE;
	}else{
		return FALSE;
	}
}

void AutoTime::start_timer(time_t now_t){
	if(!this->started){
		this->started_t=now_t;
		this->stop_t=now_t+this->duration;
		if((now_t-this->start_t) > 0){
			this->start_delta=(now_t-this->start_t);
		}		
		gmtime_r(&this->started_t,&this->started_time);
		gmtime_r(&this->stop_t,&this->stop);
		this->started=TRUE;
		this->elapsed=0;
	}
}

void AutoTime::shift_start(int32_t delta){
	this->start_delta=delta;
	this->start_t+=delta;
	this->stop_t+=delta;
	gmtime_r(&this->start_t,&this->start);
	gmtime_r(&this->stop_t,&this->stop);
}

uint8_t AutoTime::is_done(time_t now_t){
	if(this->is_running()){
		this->elapsed=difftime(now_t,this->started_t);
		if(elapsed>=this->duration){
			return TRUE;	
		}
	}
	return FALSE;
}

void AutoTime::pause(time_t now_t){
	this->paused_t=now_t;
}

void AutoTime::resume(time_t now_t){
	this->started_t+=difftime(now_t,this->paused_t);
}

uint8_t AutoTime::is_running(void){
	return this->started;
}

void AutoTime::stop_timer(uint8_t set_forward){
	this->start_t-=this->start_delta;
	if(set_forward){
		this->start_t+=ONE_DAY;
	}
	this->stop_t=this->start_t+this->duration;
	gmtime_r(&this->start_t,&this->start);
	gmtime_r(&this->stop_t,&this->stop);
	this->start_delta=0;
	this->shifted=0;
	this->elapsed=0;
	this->started=0;
}

struct tm AutoTime::get_start(){
	struct tm ret=this->start;
	return ret;
}

struct tm AutoTime::get_stop(){
	struct tm ret=this->stop;
	return ret;	
}

AutoTime& AutoTime::operator=(const AutoTime &rhs){
	this->duration=rhs.duration;
	this->elapsed=rhs.elapsed;
	this->started=rhs.started;
	this->done=rhs.done;
	this->start=rhs.start;
	this->stop=rhs.stop;
	this->started_time=rhs.started_time;
	this->started_t=rhs.started_t;
	this->start_t=rhs.start_t;
	this->stop_t=rhs.stop_t;
	return *this;
}
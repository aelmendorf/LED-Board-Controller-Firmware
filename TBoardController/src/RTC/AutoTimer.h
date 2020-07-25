/* 
* AutoTimer.h
*
* Created: 10/5/2018 10:21:58
* Author: 600076
*/


#ifndef __AUTOTIMER_H__
#define __AUTOTIMER_H__
#include <AutoTime.h>


class AutoTimer
{
//variables
public:
protected:
private:
	AutoTime *timers;
	uint8_t timer_count;
	uint8_t timer_allocated;
	int8_t running_timer,ready_timer;
	struct tm reset_tm;
	time_t reset_t;
//functions
public:
	int8_t add_time(struct tm *now,uint8_t hr,uint8_t min,uint8_t sec,uint32_t duration_secs);
	int8_t check_starts(time_t now_t);
	uint8_t is_done(time_t now_t);
	void reset_timer(uint8_t timer,struct tm *now,uint8_t hr,uint8_t min,uint8_t sec,uint32_t duration_secs);
	int8_t start_timer(time_t now_t);
	int8_t stop_timer(uint8_t set_forward);
	uint8_t is_running_timer(void);

	
	AutoTimer();
	~AutoTimer();
protected:
private:
	AutoTimer( const AutoTimer &c );
	AutoTimer& operator=( const AutoTimer &c );
}; //AutoTimer

#endif //__AUTOTIMER_H__

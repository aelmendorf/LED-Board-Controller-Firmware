/* 
* AutoTime.h
*
* Created: 10/5/2018 10:21:49
* Author: 600076
*/


#ifndef __AUTOTIME_H__
#define __AUTOTIME_H__
#include <TBoard.h>
#include <time.h>


class AutoTime
{
//variables
public:
	time_t started_t,start_t,stop_t,paused_t;
	struct tm start,stop,started_time;
	uint32_t duration,elapsed;
	int32_t start_delta;
	uint8_t started,done,shifted;
protected:
private:

//functions
public:
	AutoTime();
	~AutoTime();
	void start_timer(time_t now);
	void stop_timer(uint8_t set_forward);
	uint8_t check_start(time_t now_t);
	void set(struct tm *now,uint8_t hr,uint8_t min,uint8_t sec,uint32_t duration_secs);
/*	uint32_t get_elapsed(void);*/
/*	uint32_t get_duration(void);*/
	uint8_t is_running(void);
	uint8_t is_done(time_t now);
	void pause(time_t now_t);
	void resume(time_t now_t);
	void shift_start(int32_t delta);
	struct tm get_start(void);
	struct tm get_stop(void);
	AutoTime& operator=(const AutoTime &rhs);
protected:
private:
	AutoTime(const AutoTime &c);


}; //AutoTime

#endif //__AUTOTIME_H__

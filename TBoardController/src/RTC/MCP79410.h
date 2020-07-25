/* 
* MCP79410.h
*
* Created: 9/21/2018 8:45:02 AM
* Author: 600076
*/


#ifndef __MCP79410_H__
#define __MCP79410_H__

#include <TBoard.h>
#include <i2cmaster.h>
#include <stdlib.h>
#include <Helpers.h>
#include <time.h>

//hold decimal values of time/date
typedef struct RTCC_Data{
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
	unsigned char date;
	unsigned char month;
	unsigned char year;
	unsigned char weekday;
	inline RTCC_Data& operator=(const RTCC_Data &rhs){
		this->sec=rhs.sec;
		this->min=rhs.min;
		this->hour=rhs.hour;
		this->date=rhs.date;
		this->month=rhs.month;
		this->year=rhs.year;
		this->weekday=rhs.weekday;
		return *this;
	}
}RTCC;

typedef enum Format{H24=0,H12}Format;
typedef enum PMAM{AMT=0,PMT}PMAM;
typedef enum Alarm{ZERO=0,ONE}Alarm;
typedef enum AlarmStatus {NOT_SET = 0, SET} AlarmStatus;
typedef enum MFP_MODE{GPO=0,ALARM_INTERRUPT,SQUARE_WAVE}MFPMode;
typedef enum MFP_POL{LOWPOL=0,HIGHPOL}Polarity;
typedef enum MATCH{SEC_MATCH=0,MIN_MATCH,HR_MATCH,WEEKDAY_MATCH,DATE_MATCH,FULL_MATCH}Match;

typedef struct RTCC_TIME{
	 RTCC_Data now;
	 RTCC_Data power_up;
	 RTCC_Data power_down;
	 time_t now_t;
	 struct tm now_tm;
}RTCC_TIME;

class MCP79410
{
//variables
public:

protected:
private:
	 unsigned char err_flag;
	 RTCC_TIME *time_data;
//functions
public:
	MCP79410();
	~MCP79410();
	void initialize(uint8_t set_time);
	void set_sys_time();
	uint8_t is_running(void);
	uint8_t is_ext_osc(void);
	void enable_oscillator(uint8_t enable);
	void enable_ext_osc(uint8_t enable);
	
	//time 
	time_t Now(void);
	void get_time();
	void get_sys_time(struct tm *_tm);
	void set_sys_time(RTCC_Data new_time);
	void set_time(RTCC_Data* time);
	void set_hour_format(Format format);
	void set_AMPM(PMAM meridian);
	//alarms
	void enable_alarm(Alarm alarm,uint8_t enable);
	AlarmStatus alarm_status(Alarm alarm);
	void clear_interupt(Alarm alarm);
	void set_alarm(RTCC_Data *time,Alarm alarm);
	void set_alarm_polarity(Alarm alarm,Polarity pol);
	void set_alarm_match(Alarm alarm,Match match);
	void set_mfp_function(MFPMode mode);
	void set_mfp_gpo(Polarity status);
	//check operation
	uint8_t check_power_failure(void);
	uint8_t battery_enabled(void);
	void enable_battery(uint8_t enable);
	RTCC_Data power_up_time(void);
	RTCC_Data power_down_time(void);
	
	// i/o
	void write(unsigned char reg,unsigned char data);
	unsigned char read(unsigned char reg);
	
	//helpers
	unsigned char dec2bcd(uint8_t dec);
	uint8_t bcd2dec(unsigned char bcd);
	
	void RTCCtoTM(RTCC_Data* data,struct tm* _tm);
	void TMtoRTCC(struct tm* _tm,RTCC_Data* rtcc_t);
	
protected:
private:
	MCP79410(const MCP79410 &c);
	MCP79410& operator=( const MCP79410 &c );

}; //MCP79410

#endif //__MCP79410_H__

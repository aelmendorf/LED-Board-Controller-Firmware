/* 
* EEPROM.h
*
* Created: 9/26/2018 1:05:44 PM
* Author: 600076
*/


#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <avr/io.h>
#include <avr/eeprom.h>
#include <DateTime.h>

#define REPO_SIZE 3

typedef enum mem_index{START=0,STEP,STOP}MEM_INDEX;

typedef struct AUTO_LOG{
	DateTime stamp;
	uint8_t current;
	uint8_t running:1;
	inline AUTO_LOG& operator=(const AUTO_LOG &c){
		this->stamp=c.stamp;
		this->current=c.current;
		this->running=c.running;
		return *this;
	}
}AUTO_LOG;

/*extern uint8_t EEMEM CURRENT_ADDR;*/
extern AUTO_LOG EEMEM START_ADDR;
extern AUTO_LOG EEMEM STEP_ADDR;
extern AUTO_LOG EEMEM STOP_ADDR;

class EEPROM
{
//variables
public:

protected:

private:
	uint8_t current_value;
	AUTO_LOG in_memory[REPO_SIZE];
//functions
public:
	EEPROM();
	~EEPROM();
	void initalize(void);
	void auto_log(const AUTO_LOG &log_itm,const MEM_INDEX &in);
	uint8_t read_current(void);
	void set_current(const uint8_t &val);

protected:
private:
	EEPROM( const EEPROM &c );
	EEPROM& operator=( const EEPROM &c );

}; //EEPROM

#endif //__EEPROM_H__

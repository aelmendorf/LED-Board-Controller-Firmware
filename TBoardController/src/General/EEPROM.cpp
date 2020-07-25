/* 
* EEPROM.cpp
*
* Created: 9/26/2018 1:05:44 PM
* Author: 600076
*/


#include "EEPROM.h"

/*extern uint8_t EEMEM CURRENT_ADDR;*/
extern AUTO_LOG EEMEM START_ADDR;
extern AUTO_LOG EEMEM STEP_ADDR;
extern AUTO_LOG EEMEM STOP_ADDR;

// default constructor
EEPROM::EEPROM()
{
	this->current_value=0;
} //EEPROM

// default destructor
EEPROM::~EEPROM()
{
	
} //~EEPROM

void EEPROM::initalize(){
/*	this->current_value=eeprom_read_byte(&CURRENT_ADDR);*/
	eeprom_read_block(&this->in_memory[START],&START_ADDR,sizeof(AUTO_LOG));
	eeprom_read_block(&this->in_memory[STEP],&STEP_ADDR,sizeof(AUTO_LOG));
	eeprom_read_block(&this->in_memory[STOP],&STOP_ADDR,sizeof(AUTO_LOG));
}

void EEPROM::auto_log(const AUTO_LOG &log_itm,const MEM_INDEX &in){ 
	this->in_memory[in]=log_itm;
	switch(in){
		case START:{
			eeprom_update_block(&this->in_memory[in],&START_ADDR,sizeof(AUTO_LOG));
			break;
		}
		case STEP:{
			eeprom_update_block(&this->in_memory[in],&STEP_ADDR,sizeof(AUTO_LOG));
			break;
		}
		case STOP:{
			eeprom_update_block(&this->in_memory[in],&STOP_ADDR,sizeof(AUTO_LOG));
			break;
		}
	}
}

uint8_t EEPROM::read_current(){
	return this->current_value;
}

void EEPROM::set_current(const uint8_t &val){
	this->current_value=val;
/*	eeprom_update_byte(&CURRENT_ADDR,val);*/
}

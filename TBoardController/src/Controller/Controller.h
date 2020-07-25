#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__
#include <TBoard.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <math.h>
#include <Helpers.h>
#include <CurrentDriver.h>
#include <Switch.h>
#include <Timer.h>
#include <MCP79410.h>
#include <AutoTime.h>
#include <time.h>

/************************************************************************/
/* Enums Mode and State.                                                */
/* Mode controlled by switch state                                      */
/* Each mode has independent state of                                   */                            
/************************************************************************/
				//0    1   2    3       4        5      6
typedef enum modes{ON,OFF,AUTO,PROGRAM,LID_OPEN,SLEEP,NOMODE}MODE;
typedef enum state{RUNNING,DONE,STOPPED,ERROR,IDLE,INIT,NOSTATE}STATE;	
typedef enum p_modes{INPUT,WAIT_OFF,DISPLAY,EXIT}P_MODE;

/************************************************************************/
/* struct LED_SEQ
/*   used in led blink sequence for displaying current value                                                                  
/************************************************************************/
typedef struct LED_SEQ{
	uint8_t red_led;
	uint8_t blue_led;
	uint8_t green_led;
	uint8_t current_value;
}LED_SEQ;

/************************************************************************/
/* struct SwitchSeq
/*   Used for counting switch and checking for sequence
/************************************************************************/

typedef struct SwitchSeq{
	uint8_t pos1;
	uint8_t pos2;
	uint8_t pos3;
	inline bool operator==(const SwitchSeq &rhs)const
	{
		return this->pos1==rhs.pos1 && this->pos2==rhs.pos2 && this->pos3==rhs.pos3;
	}
}SwitchSeq;

/************************************************************************/
/* struct Task
/*   what the program is tasked with doing 
/************************************************************************/

typedef struct Task{
	uint8_t latched:1;
	uint8_t continue_task:1;
	STATE state;
	MODE  mode;
	Task(){
		this->latched=0;
		this->state=NOSTATE;
		this->mode=NOMODE;
	}
	inline void clear(){
		this->latched=0;
		this->state=NOSTATE;
		this->mode=NOMODE;
	}
	inline bool operator==(const Task &rhs)const
	{
		return this->mode==rhs.mode;
	}
	inline bool operator!=(const Task &rhs)const
	{
		return this->mode!=rhs.mode;
	}
	inline Task& operator=(const Task &rhs){
		this->latched=rhs.latched;
		this->state=rhs.state;
		this->mode=rhs.mode;
		return *this;
	}
}Task;

/*
 class Controller                                                      
	As name suggest is the Controller.  All logic,processes,etc 
	contained here
*/

class Controller
{
//variables
public:
protected:
private:
	SwitchSeq sequences[3]={{0,4,3},{1,0,0},{0,0,1}};
	LED_SEQ led_seq[LEDSEQCOUNT]={{1,2,3,5},{1,3,2,7},{2,1,3,10},{3,1,2,15},{2,3,1,17},{3,2,1,20}};
	AutoTime auto_time,auto_cont;
	P_MODE p_mode,p_next;
	MCP79410 rtc;
	Task task,next_task,saved_task;
	Timer *timer;
	CurrentDriver *current_driver;
	Switch switches[4];
	SwitchSeq sequence;
	time_t reset_time_t;
	struct tm reset_tm;
	uint8_t first_on,display_done,set_current,display_index;
	uint16_t display_count,millis;
	uint8_t toggle:1;
	uint8_t current_set:1;
	uint8_t sequence_set:1;
	uint8_t BLINK_TMR,LATCH_TMR,CHECK_TMR,SLEEP_TMR,AUTO_TMR;
	uint8_t* eeprom_address;
	AutoTime* log_address;
	volatile uint16_t *UVOFF_LED;
	volatile uint16_t *UVON_LED;
//functions
public:
	Controller();
	Controller(Timer *tmr,CurrentDriver *cdriver,uint8_t* address);
	~Controller();
	void start();
	void run();
	void setup(uint8_t set_time);
	void check_switchs(void);
	void show_current(void);
	void switches_program(void);
	void process_switch_prgm(int sw);
	void idle_blink(volatile uint16_t* reg);
	/************************************************************************/
	/* Main Modes                                                           */
	/************************************************************************/
	void On(void);
	void Off(void);
	void Auto(void);
	void Program(void);
	void LidOpen(void);
	void Sleep(void);

protected:
private:
	void setup_io(void);
	void setup_auto_time(void);
	void setup_pwm();
	void setupCallTable();
	void setupTimers(Timer *timer);
	void run_uvoff(void);
	void run_red(void);
	void run_uvon(void);
	void run_mesh(void);
	void setup_led_sequence(void);
	void run_program(void);
	void all_led_off(void);
	void clear_sequence(void);
	void process_switch(int sw);
	void process_task(void);
	void state_transition();
	void check_sequence(int x);
	
	typedef void (Controller::*Fptr)(void);
	typedef	void (Controller::*POT_LED)(void);
	typedef	void (Controller::*LSEQ)(void);
	
	LSEQ led_func[LEDSEQSTEPS];
	POT_LED pot_func[4];
	Fptr func[6];//={&Controller::On,&Controller::Auto,&Controller::Off,&Controller::Program};
	
	Controller( const Controller &c );//not used, no need to copy
	Controller& operator=( const Controller &c );//not used no need to set equal(copy)
}; //Controller

#endif //__CONTROLLER_H__

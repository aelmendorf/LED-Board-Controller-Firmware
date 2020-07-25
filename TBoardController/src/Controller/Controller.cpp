#include "Controller.h"

Controller::Controller(Timer *timer,CurrentDriver *cdriver,uint8_t* address){
	this->current_driver=cdriver;
	this->setupTimers(timer);
	this->setupCallTable();

	this->UVOFF_LED=&OCR1B;
	this->UVON_LED=&OCR1A;
	this->display_index=0;
	this->toggle=0;
	this->p_mode=INPUT;
	this->eeprom_address=address;
}

Controller::Controller()
{
	this->UVOFF_LED=&OCR1B;
	this->UVON_LED=&OCR1A;
	this->func[ON]=&Controller::On;
	this->func[AUTO]=&Controller::Auto;
	this->func[OFF]=&Controller::Off;
	this->func[PROGRAM]=&Controller::Program;
	this->func[SLEEP]=&Controller::Sleep;
	this->func[LID_OPEN]=&Controller::LidOpen;
	this->toggle=0;
}

void Controller::run(){
	while(1){
		this->check_switchs();
	}
}

void Controller::setupTimers(Timer *timer){
	this->timer=timer;
	this->BLINK_TMR=0;
	this->LATCH_TMR=this->timer->add_timer(LATCH_PERIOD);
	this->CHECK_TMR=this->timer->add_timer(CHECK_PERIOD);
	this->AUTO_TMR=this->timer->add_timer(AUTO_BLINK);
}

void Controller::setupCallTable(){
	this->func[ON]=&Controller::On;
	this->func[AUTO]=&Controller::Auto;
	this->func[OFF]=&Controller::Off;
	this->func[PROGRAM]=&Controller::Program;
	this->func[SLEEP]=&Controller::Sleep;
	this->func[LID_OPEN]=&Controller::LidOpen;
	this->pot_func[RED_POS]=&Controller::run_red;
	this->pot_func[GREEN_POS]=&Controller::run_uvoff;
	this->pot_func[BLUE_POS]=&Controller::run_uvon;
	this->pot_func[ORANGE_POS]=&Controller::run_mesh;
}

void Controller::setup(uint8_t set_time){
	this->setup_io();
	this->setup_pwm();
	this->current_driver->init();
	this->rtc.initialize(set_time);
	sei();
}

void Controller::start(){
	if(!(PIND & (1<<POS1))){
		this->task.mode=AUTO;
		this->task.latched=0;
	}else if(!(PIND & (1<<POS2))){
		this->task.mode=OFF;
		this->task.latched=0;
	}else if(!(PIND & (1<<POS3))){
		this->task.mode=ON;
		this->task.latched=0;
	}else{
		this->task.mode=OFF;
		this->task.latched=0;
	}
	this->sequence={0,0,0};
	this->set_current=eeprom_read_byte(this->eeprom_address);
	this->current_set=1;

	int8_t value=this->current_driver->set_current_value(this->set_current);
	if(value!=-1){
		this->display_index=value;
	}else{
		this->set_current=DEFAULT_CURRENT;
		this->current_set=1;
		this->display_index=this->current_driver->set_current_value(this->set_current);
	}
	time_t now_t;
	struct tm now_tm;
	now_t=rtc.Now();
	gmtime_r(&now_t,&now_tm);
	this->auto_time.set(&now_tm,AUTO_HR,AUTO_MIN,0,AUTO_DURATION);
	this->setup_led_sequence();
	(this->*func[this->task.mode])();
}

void Controller::setup_auto_time(){
	time_t now_t;
	struct tm now_tm;
	now_t=rtc.Now();
	gmtime_r(&now_t,&now_tm);
	this->auto_time.set(&now_tm,now_tm.tm_hour,now_tm.tm_min+1,0,30);
}

void Controller::setup_io(){
	//disable JTAG
	MCUCR|=(1<<JTD);
	//_NOP();
	MCUCR|=(1<<JTD);
	
	//CLKPR|=(1<<CLKPCE);
	//CLKPR&=~(1<<CLKPS0) & ~(1<<CLKPS1) &~(1<<CLKPCE);
	CLKPR=0x80;
	CLKPR=0;
	
	//Signal LEDS
	DDRB|=(1<<RED) | (1<<GREEN) | (1<<BLUE);
	PORTB|=(1<<RED) | (1<<GREEN) | (1<<BLUE);
	
	//setup switch
	this->switches[POS1_B]=Switch(&PIND,POS1,LOW);
	this->switches[POS2_B]=Switch(&PIND,POS2,LOW);
	this->switches[POS3_B]=Switch(&PIND,POS3,LOW);
	this->switches[LID_B]=Switch(&PINF,LID_SWITCH,HIGH);
		
	//3POS switch. set inputs
	PORTD|=(1<<POS1) |(1<<POS2) |(1<<POS3);//Enable pullup*/
	DDRD&=~(1<<POS1) & ~(1<<POS2) & ~(1<<POS3);
	
	//PORTF|=(1<<LID_SWITCH);
	DDRF&=~(1<<LID_SWITCH) & ~(1<<POT); /*& ~(1<<VBAT);*/
	//Configure ADC
}

void Controller::setup_pwm(){
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11)|(1<<WGM10);
	TCCR1B = (1<<WGM12)|(1<<CS10);
	OCR1A = PWM_OFF;// LED initially off
	OCR1B=PWM_OFF;
	TIMSK1 |=(1<<TOIE1); // Enable overflow interrupt
}

void Controller::check_switchs(){
	for(int i=0;i<3;i++){
		this->switches[i].debounce();
		if(this->switches[i].switched()){
			this->switches[i].clear_state();
			this->process_switch(i);
		}
		_delay_ms(5);
	}//end
}//End check mode

void Controller::switches_program(){
	for(int i=0;i<4;i++){
		this->switches[i].debounce();
		if(this->switches[i].switched()){
			this->switches[i].clear_state();
			this->process_switch_prgm(i);
		}
		_delay_ms(5);
	}//end
}

void Controller::process_switch_prgm(int sw){
	switch(sw){
		case POS1_B:{
			this->p_next=EXIT;
			break;
		}
		case POS2_B:{
			if(this->p_mode==WAIT_OFF){
				this->p_next=DISPLAY;
			}else{
				this->p_next=this->p_mode;;
			}
			break;
		}
		case POS3_B:{
			if(this->p_mode==INPUT){
				this->p_next=WAIT_OFF;
			}else{
				this->p_next=this->p_mode;
			}
			break;
		}
		case LID_B:{
				
			break;
		}
		case -1:{
			break;
		}
	}
}

void Controller::process_switch(int sw){
		switch(sw){
			case POS1_B:{
				this->clear_sequence();
				this->next_task.mode=AUTO;
				this->next_task.latched=0;
				break;
			}
			case POS2_B:{
 				if(this->sequence.pos2==4 && this->sequence.pos3==3){
 					this->next_task.mode=PROGRAM;
 					this->next_task.latched=0;
 				}else{
 					this->next_task.mode=OFF;
 					this->next_task.latched=0;
 				}
				this->next_task.mode=OFF;
				this->next_task.latched=0;
				this->clear_sequence();
				break;
			}
			case POS3_B:{
				this->next_task.mode=ON;
				this->next_task.latched=0;
				break;
			}
		}
	(this->*func[this->task.mode])();
}//End process switch

void Controller::clear_sequence(){
	this->sequence.pos1=0;
	this->sequence.pos2=0;
	this->sequence.pos3=0;
}

void Controller::setup_led_sequence(){
	this->led_func[0]=this->pot_func[this->display_index];
	this->led_func[1]=&Controller::all_led_off;
	this->led_func[2]=this->pot_func[this->display_index];
	this->led_func[3]=&Controller::all_led_off;
 	int value=this->set_current;
 	int arr[LEDSEQCOUNT];
 	for(int i=0;i<LEDSEQCOUNT;i++){
 		arr[i]=this->led_seq[i].current_value;
 	}
 	char *fraction=0;
 	int index=BinarySearch(arr,value,fraction,LEDSEQCOUNT);
 	if(index>=0 && index<=LEDSEQCOUNT){
 		LED_SEQ seq=this->led_seq[index];
 		this->led_func[0]=&Controller::run_mesh;
 		this->led_func[1]=&Controller::all_led_off;
 		this->led_func[2]=&Controller::run_mesh;
 		this->led_func[3]=&Controller::all_led_off;
 		this->led_func[seq.red_led+3]=&Controller::run_red;
 		this->led_func[seq.blue_led+3]=&Controller::run_uvon;
 		this->led_func[seq.green_led+3]=&Controller::run_uvoff;
 		this->led_func[7]=&Controller::all_led_off;
 	}
}

void Controller::On(){
	if(this->task!=this->next_task){
		this->task.state=STOPPED;
	}
	if(!this->task.latched){
		this->task.state=INIT;
		this->task.latched=1;
		this->display_count=0;
		this->display_done=0;
		if(this->sequence.pos2>=1){
			this->sequence.pos3++;
		}
		this->timer->set_delay(LATCH_TMR,1200);
		this->timer->start(LATCH_TMR);
		this->timer->set_delay(BLINK_TMR,750);
		this->timer->start(BLINK_TMR);
	}
	switch(this->task.state){
		case INIT:{
			if(this->timer->is_triggered(LATCH_TMR)){
				this->display_count=0;
				this->display_done=1;
				this->task.state=RUNNING;
				this->current_driver->turn_on_manual(ON_CURRENT);
				this->run_uvon();
			}
			break;
		}
		case RUNNING:{
			if(PINF & (1<<LID_SWITCH)){
				this->task.state=ERROR;
				this->current_driver->turn_off();
				this->timer->set_delay(BLINK_TMR,500);
				this->timer->start(BLINK_TMR);
				this->run_red();
			}
			break;
		}
		case STOPPED:{
			this->current_driver->turn_off();
			this->all_led_off();
			this->task=this->next_task;
			this->task.latched=0;
			(this->*func[this->task.mode])();
			break;
		}
		case IDLE:{
			break;
		}
		case ERROR:{
			if(!(PINF & (1<<LID_SWITCH))){
				this->task.state=RUNNING;
				this->current_driver->turn_on_manual(ON_CURRENT);
				this->run_uvon();
			}else{
				if(this->timer->is_triggered(BLINK_TMR)){
					PORTB^=(1<<RED);
					this->timer->reset(BLINK_TMR);
				}
			}
			break;
		}
		case DONE:{
			break;
		}
	}
}

void Controller::Auto(){
	if(this->task!=this->next_task){
		this->saved_task=this->task;
		this->task.state=STOPPED;
	}
	if(!this->task.latched){
		this->task.state=INIT;
		this->task.latched=1;
		this->timer->set_delay(LATCH_TMR,1200);
		this->timer->start(LATCH_TMR);
		this->clear_sequence();			
	}
	switch(this->task.state){
		case INIT:{
			if(this->timer->is_triggered(LATCH_TMR)){
				time_t now_t=this->rtc.Now();
				if(this->auto_time.check_start(now_t)){
					this->auto_time.start_timer(now_t);
					this->task.state=RUNNING;
					this->current_driver->turn_on_manual(AUTO_CURRENT);
					this->timer->set_delay(CHECK_TMR,1000);
					this->timer->start(CHECK_TMR);
					this->run_uvon();
				}else{
					this->run_uvoff();
					this->task.state=IDLE;	
				}
			}
			break;
		}
		case RUNNING:{
			time_t now_t=this->rtc.Now();
			if(PINF & (1<<LID_SWITCH)){
				this->auto_time.pause(now_t);
				this->run_red();
				this->current_driver->turn_off();
				this->saved_task=task;
				this->task.state=ERROR;
				this->timer->set_delay(BLINK_TMR,500);
				this->timer->start(BLINK_TMR);
			}else{
				if(this->auto_time.is_running()){
					if(auto_time.is_done(now_t)){
						auto_time.stop_timer(TRUE);
						this->current_driver->turn_off();
						this->task.state=IDLE;
						this->run_uvoff();
					}else{
						this->task.state=RUNNING;
					}
				}else{
					this->run_uvoff();
					this->task.state=IDLE;
				}
			}
			break;
		}
		case STOPPED:{
			if(this->auto_time.is_running()){
				this->auto_time.stop_timer(TRUE);
			}
			this->current_driver->turn_off();
			this->all_led_off();
			this->task=this->next_task;
			this->task.latched=0;
			(this->*func[this->task.mode])();
			break;
		}
		case IDLE:{
			if(PINF & (1<<LID_SWITCH)){
				this->run_red();
				this->saved_task=task;
				this->task.state=ERROR;
				this->timer->set_delay(BLINK_TMR,500);
				this->timer->start(BLINK_TMR);
			}else{
				time_t now_t=this->rtc.Now();
				if(this->auto_time.check_start(now_t)){
					this->auto_time.start_timer(now_t);
					this->task.state=RUNNING;
					this->current_driver->turn_on_manual(AUTO_CURRENT);
					this->run_uvon();
				}else{
					this->task.state=IDLE;
				}
			}
			break;
		}
		case ERROR:{
			if(!(PINF & (1<<LID_SWITCH))){
				if(this->saved_task.state==RUNNING){
					this->task.state=RUNNING;
					this->current_driver->turn_on_manual(AUTO_CURRENT);
					this->run_uvon();
					this->auto_time.resume(rtc.Now());
				}else if(this->saved_task.state==IDLE){
					this->task.state=IDLE;
					this->run_uvoff();
				}
			}else{
				if(this->timer->is_triggered(BLINK_TMR)){
					PORTB^=(1<<RED);
					this->timer->reset(BLINK_TMR);
				}
			}
			break;
		}
		case DONE:{
			
			break;
		}
	}
}

void Controller::idle_blink(volatile uint16_t* reg){
	if(this->timer->is_triggered(AUTO_TMR)){
		if(this->toggle){
			*reg=PWM_OFF;
		}else{
			*reg=AUTO_PWM_IDLE;
		}
		this->toggle^=1;
		this->timer->reset(AUTO_TMR);
	}
}

void Controller::Program(){
	if(this->task!=this->next_task){
		this->task.state=STOPPED;
	}
	if(!this->task.latched){
		this->task.latched=1;
		this->task.state=INIT;
		this->display_count=0;
		this->display_done=0;
		this->timer->set_delay(BLINK_TMR,500);
		this->timer->start(BLINK_TMR);
	}
	switch(this->task.state){
		case INIT:{
			if(!display_done){
				if(this->timer->is_triggered(BLINK_TMR)){
					(this->*led_func[this->display_count])();
					this->timer->reset(BLINK_TMR);
					this->display_count++;
					if(this->display_count>=LEDSEQSTEPS){
						this->display_count=0;
						this->display_done=1;
						this->task.state=RUNNING;
					}
				}
			}
			break;
		}
		case RUNNING:{
			uint8_t done=0;
			uint8_t count=0;
			this->p_mode=INPUT;
			this->p_next=this->p_mode;
			do{
				if(this->p_mode!=this->p_next){
					switch(this->p_mode){
						case INPUT:{
							this->p_mode=this->p_next;
							this->all_led_off();
							break;
						}
						case WAIT_OFF:{
							if(this->p_next==DISPLAY){
								this->timer->set_delay(BLINK_TMR,500);
								this->timer->start(BLINK_TMR);
							}
							this->p_mode=this->p_next;
							break;
						}
						case DISPLAY:{
							this->p_mode=this->p_next;
							break;
						}
						case EXIT:{
							this->p_mode=this->p_next;
							break;
						}
					}
				}
				switch(this->p_mode){
					case INPUT:{
						this->set_current=this->current_driver->read_input();
						this->display_index=this->current_driver->get_display();
						(this->*pot_func[this->display_index])();
						this->current_set=1;
						this->setup_led_sequence();
						break;
					}
					case WAIT_OFF:{//wait for switch back
						break;
					}
					case DISPLAY:{
 						if(this->timer->is_triggered(BLINK_TMR)){
 							(this->*led_func[count])();
 							this->timer->reset(BLINK_TMR);
 							count++;
 							if(count>=4){
 								count=0;
 								this->p_mode=INPUT;
								eeprom_write_byte(this->eeprom_address,this->set_current); 
 							}
 						}
						break;
					}
			
					case EXIT:{
						done=1;
						break;
					}
				}
				this->switches_program();
			}while(!done);
			break;
		}
		case STOPPED:{
			this->clear_sequence();
			this->first_on=1;
			this->task.mode=OFF;
			this->task=this->next_task;
			this->task.latched=0;
			this->all_led_off();
			(this->*func[this->task.mode])();
			break;
		}
		case IDLE:{
			break;
		}
		case ERROR:{
			break;
		}
		case DONE:{
			break;
		}
	}	
}

void Controller::Off(){
	if(this->task!=this->next_task){
		this->task.state=STOPPED;
	}
	if(!this->task.latched){
		if(!this->first_on){
			this->sequence.pos2++;
		}
		this->task.state=INIT;
		this->task.latched=1;		
		this->timer->start(LATCH_TMR);
	}
	switch(this->task.state){
		case INIT:{
			if(first_on){			
				this->run_red();
				uint8_t wait_done=0;
				if(!(PIND & (1<<POS2)))
					this->first_on=0;
						
				while((PIND & (1<<POS2))){
					if(this->timer->is_triggered(LATCH_TMR) && !wait_done){
						wait_done=1;
						this->timer->set_delay(BLINK_TMR,500);
						this->timer->start(BLINK_TMR);
					}
					if(wait_done){
						if(this->timer->is_triggered(BLINK_TMR)){
							this->timer->reset(BLINK_TMR);
							PORTB^=(1<<RED);
						}
					}//end
				}//done waiting for switch
				this->first_on=0;
				this->task.state=RUNNING;
				this->run_red();
			}else{
				this->task.state=RUNNING;
				if(!(this->sequence.pos2==4 && this->sequence.pos3==3)){
					this->run_red();
				}
				this->timer->set_delay(SLEEP_TMR,30000);
				this->timer->start(SLEEP_TMR);
			}
			break;
		}
		case RUNNING:{
			if(this->timer->is_triggered(SLEEP_TMR)){
				this->timer->set_delay(AUTO_TMR,AUTO_BLINK);
				this->timer->start(AUTO_TMR);
				this->task.state=IDLE;
			}
			break;
		}
		case STOPPED:{
			this->all_led_off();
			this->task=this->next_task;
			this->task.latched=0;
			(this->*func[this->task.mode])();
			break;
		}
		case IDLE:{
			if(this->timer->is_triggered(AUTO_TMR)){
				PORTB^=(1<<RED);
				this->timer->reset(AUTO_TMR);
			}
			break;
		}
		case ERROR:{
			break;
		}
		case DONE:{
			break;
		}
	}
}

void Controller::LidOpen(){
	if(this->task!=this->next_task){
		this->task.state=STOPPED;
	}
	if(!this->task.latched){
		this->task.state=INIT;
		this->task.latched=1;
		this->timer->set_delay(BLINK_TMR,1000);
		this->timer->start(BLINK_TMR);
	}
	switch(this->task.state){
		case INIT:{
			this->run_red();
			uint8_t wait_done=0;
			while(!(PINF & (1<<LID_SWITCH))){
				if(this->timer->is_triggered(LATCH_TMR) && !wait_done){
					wait_done=1;
					this->timer->set_delay(BLINK_TMR,500);
					this->timer->start(BLINK_TMR);
				}
				if(wait_done){
					if(this->timer->is_triggered(BLINK_TMR)){
						this->timer->reset(BLINK_TMR);
						PORTB^=(1<<RED);
					}
				}//end
			}//done waiting for switch
			break;
		}
		case RUNNING:{
			break;
		}
		case STOPPED:{
			this->all_led_off();
			this->task=this->saved_task;
			this->task.continue_task=1;
			(this->*func[this->task.mode])();
			break;
		}
		case IDLE:{
			break;
		}
		case ERROR:{
			break;
		}
		case DONE:{
			break;
		}
	}
}

void Controller::all_led_off(){
	OCR1A=PWM_OFF;
	OCR1B=PWM_OFF;
	PORTB|=(1<<RED);
}

void Controller::run_red(){
	PORTB&=~(1<<RED);
	OCR1A=PWM_OFF;
	OCR1B=PWM_OFF;

}//End run_off

void Controller::run_uvoff(){
	OCR1A=ON_PWM;
	OCR1B=PWM_OFF;
	PORTB|=(1<<RED);

}

void Controller::run_uvon(){
	OCR1B=AUTO_PWM;
	OCR1A=PWM_OFF;
	PORTB|=(1<<RED);
}

void Controller::run_mesh(){
	OCR1B=1023;
	OCR1A=700;
	PORTB&=~(1<<RED);
}

// default destructor
Controller::~Controller()
{
	//nothing to free
} //~Controller

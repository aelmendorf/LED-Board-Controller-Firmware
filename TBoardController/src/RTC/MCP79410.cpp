#include "MCP79410.h"

// default constructor
MCP79410::MCP79410()
{
	this->time_data=(RTCC_TIME*)malloc(sizeof(RTCC_TIME));

} //MCP79410

// default destructor
MCP79410::~MCP79410()
{
	if(this->time_data!=NULL){
		free(this->time_data);
	}

} //~MCP79410

void MCP79410::initialize(uint8_t set_time){
	i2c_init();
	if(set_time){
		this->set_sys_time();	
	}
}

void MCP79410::set_sys_time(){
	unsigned char day=0;
	day=this->read(ADDR_DAY);
	this->write(ADDR_DAY,day|VBATEN); //enable battery
	this->write(ADDR_CTRL,SQWE+ALM_NO+MFP_32K);//
	this->write(ADDR_MIN,SET_MIN);
	this->write(ADDR_HOUR,SET_HOUR);
	this->write(ADDR_DATE,SET_DATE);
	this->write(ADDR_MNTH,SET_MONTH);
	this->write(ADDR_YEAR,SET_YEAR);
	this->write(ADDR_SEC,START_32KHZ);
}

void MCP79410::set_sys_time(RTCC_Data new_time){
	unsigned char day=0;
	day=this->read(ADDR_DAY);
	this->write(ADDR_DAY,day|VBATEN); //enable battery
	this->write(ADDR_CTRL,SQWE+ALM_NO+MFP_01H);//
	this->write(ADDR_MIN,dec2bcd(new_time.min));
	this->write(ADDR_HOUR,dec2bcd(new_time.hour));
	this->write(ADDR_DATE,dec2bcd(new_time.date));
	this->write(ADDR_MNTH,dec2bcd(new_time.month));
	this->write(ADDR_YEAR,dec2bcd(new_time.year));
	this->write(ADDR_SEC,START_32KHZ);
}

void MCP79410::enable_oscillator(uint8_t enable){
	unsigned char st_bit=this->read(ADDR_DAY);
	if(enable){
		st_bit=st_bit|START_32KHZ;
	}else{
		st_bit=st_bit& ~START_32KHZ;
	}
	this->write(ADDR_SEC,st_bit);
}

void MCP79410::enable_ext_osc(uint8_t enable){
	unsigned char ex_bit=this->read(ADDR_CTRL);
	if(enable){
		ex_bit=ex_bit|EXTOSC;
	}else{
		ex_bit=ex_bit & ~EXTOSC;
	}
	this->write(ADDR_CTRL,EXTOSC);
}

uint8_t MCP79410::is_ext_osc(){
	unsigned char ext_bit=this->read(ADDR_CTRL);
	if((ext_bit & EXTOSC)==EXTOSC){
		return TRUE;
	}else{
		return FALSE;
	}
}

uint8_t MCP79410::is_running(void){
	unsigned char mask=read(ADDR_DAY);
	if((mask&OSCON)==OSCON){
		return TRUE;
	}else{
		return FALSE;
	}
}

void MCP79410::set_time(RTCC_Data* time){
 	unsigned char sec=read(ADDR_SEC);
 	unsigned char min=0;
 	unsigned char hour=read(ADDR_HOUR);
 	unsigned char date=0;
 	unsigned char month=read(ADDR_MNTH);
 	unsigned char year=0;
 	unsigned char weekday=read(ADDR_DAY);
	if((sec&START_32KHZ)==START_32KHZ){
		sec=dec2bcd(time->sec)|START_32KHZ;
	}else{
		sec=dec2bcd(time->sec);
	}
	min=dec2bcd(time->min);
	if((hour&HOUR_12)==HOUR_12){
		hour=dec2bcd(time->hour)|HOUR_12;
	}else{
		hour=dec2bcd(time->hour);
	}
	
	if((hour & PM)==PM){
		hour=hour|PM;
	}
	weekday&=0x38;
	weekday|=dec2bcd(time->weekday);
	
	date=dec2bcd(time->date);
	if((month&LP)==LP){
		month=dec2bcd(time->month)|LP;
	}else{
		month=dec2bcd(time->month);
	}
	year=dec2bcd(time->year);
	this->write(ADDR_YEAR,year);
	this->write(ADDR_MNTH,month);
	this->write(ADDR_DATE,date);
	this->write(ADDR_DAY,weekday);
	this->write(ADDR_HOUR,hour);
	this->write(ADDR_MIN,min);
	this->write(ADDR_SEC,sec);
}//

void MCP79410::get_time(){
	this->time_data->now.sec=bcd2dec(this->read(ADDR_SEC)&(~START_32KHZ));
	this->time_data->now.min=bcd2dec(this->read(ADDR_MIN));
	
	unsigned char hour_t=this->read(ADDR_HOUR);
	hour_t=((hour_t&HOUR_12)==HOUR_12)?(hour_t&0x1f):(hour_t & 0x3f);
	
	this->time_data->now.hour=bcd2dec(hour_t);
	this->time_data->now.date=bcd2dec(this->read(ADDR_DATE));
	this->time_data->now.weekday=bcd2dec(this->read(ADDR_DAY) & ~(OSCON|PWRFAIL|VBATEN));
	this->time_data->now.month=bcd2dec(this->read(ADDR_MNTH)&(~LP));
	this->time_data->now.year=bcd2dec(this->read(ADDR_YEAR));
	struct tm raw;
	this->RTCCtoTM(&this->time_data->now,&raw);
	time_t current_t=mk_gmtime(&raw);
	this->time_data->now_t=current_t;
	this->time_data->now_tm=raw;
}

time_t MCP79410::Now(){
	this->get_time();
	time_t current_t=this->time_data->now_t;
	return current_t;
}

void MCP79410::set_hour_format(Format format){
	this->enable_oscillator(FALSE);//disable osc
	unsigned char format_bit=this->read(ADDR_HOUR);
	if(format==H24){
		format_bit&=~HOUR_12; //set to military
	}else{
		format_bit|=HOUR_12;  //set to H12
	}
	this->write(ADDR_HOUR,format_bit);
	this->enable_oscillator(TRUE);
}

void MCP79410::set_AMPM(PMAM meridian){
	this->enable_oscillator(FALSE);//disable osc
	unsigned char m_bit=this->read(ADDR_HOUR);
	if(meridian==AMT){
		m_bit&=~PM; //set am
	}else{
		m_bit|=PM;  //set PM
	}
	this->write(ADDR_HOUR,m_bit);
	this->enable_oscillator(TRUE);
}

void MCP79410::enable_alarm(Alarm alarm,uint8_t enable){
	unsigned char ctrl_bits=this->read(ADDR_CTRL);
	if(alarm==ZERO){
		if(enable){
			ctrl_bits|=ALM_0;
		}else{
			ctrl_bits&=~ALM_0;
		}
	}else{
		if(enable){
			ctrl_bits|=ALM_1;
		}else{
			ctrl_bits&=~ALM_1;
		}		
	}
	this->write(ADDR_CTRL,ctrl_bits);
}

AlarmStatus MCP79410::alarm_status(Alarm alarm){
	AlarmStatus status;
	unsigned char stat;
	
	if(alarm==ZERO){
		stat=this->read(ADDR_ALM0CTL);
	}else{
		stat=this->read(ADDR_ALM0CTL);
	}
	return status=(AlarmStatus)((stat & ALMx_IF)==ALMx_IF)?SET:NOT_SET;
}

void MCP79410::clear_interupt(Alarm alarm){
	unsigned char temp;
	if(alarm==ZERO){
		temp=this->read(ADDR_ALM0CTL);
		temp&=(~ALMx_IF);
		this->write(ADDR_ALM0CTL,temp);
	}else{
		temp=this->read(ADDR_ALM1CTL);
		temp&=(~ALMx_IF);
		this->write(ADDR_ALM1CTL,temp);
	}
}

void MCP79410::set_alarm(RTCC_Data *time,Alarm alarm){
	unsigned char sec=dec2bcd(time->sec);
	unsigned char min=dec2bcd(time->min);
	unsigned char hour=dec2bcd(time->hour);
	unsigned char date=dec2bcd(time->date);
	unsigned char month=dec2bcd(time->month);
	unsigned char weekday=dec2bcd(time->weekday);
	
	if(alarm==ZERO){
		this->write(ADDR_ALM0SEC,sec|START_32KHZ);
		this->write(ADDR_ALM0MIN,min);
		this->write(ADDR_ALM0HR,hour);		
		this->write(ADDR_ALM0CTL,weekday);	
		this->write(ADDR_ALM0DAT,date);
		this->write(ADDR_ALM0MTH,month);
	}else{
		this->write(ADDR_ALM1SEC,sec|START_32KHZ);
		this->write(ADDR_ALM1MIN,min);
		this->write(ADDR_ALM1HR,hour);
		this->write(ADDR_ALM1CTL,weekday);
		this->write(ADDR_ALM1DAT,date);
		this->write(ADDR_ALM1MTH,month);
	}
}

void MCP79410::set_alarm_polarity(Alarm alarm,Polarity pol){
	unsigned char pbit=0;
	if(alarm==ZERO){
		pbit=this->read(ADDR_ALM0CTL);
	}else{
		pbit=this->read(ADDR_ALM1CTL);
	}
	
	if(pol==LOWPOL){
		pbit&=~ALMx_POL;
	}else{
		pbit|=ALMx_POL;		
	}
	
	if(alarm==ZERO){
		this->write(ADDR_ALM0CTL,pbit);
	}else{
		this->write(ADDR_ALM1CTL,pbit);
	}
}

void MCP79410::set_mfp_gpo(Polarity status){
	unsigned char gpo_bit=this->read(ADDR_CTRL);
	if(status==LOWPOL){
		gpo_bit=OUT_PIN;
	}else{
		gpo_bit|=OUT_PIN;
	}
	this->write(ADDR_CTRL,gpo_bit);
}

void MCP79410::set_alarm_match(Alarm alarm,Match match){
	unsigned char alarm_reg = 0;
	if(alarm == ZERO)
	{
		alarm_reg = ADDR_ALM0CTL;
	}else{
		alarm_reg = ADDR_ALM0CTL;
	}

	unsigned char match_bits = read(alarm_reg);

	switch(match)
	{
		case SEC_MATCH:
			match_bits &= ~(ALM0MSK2|ALM0MSK1|ALM0MSK0);
			this->write(alarm_reg,match_bits);   //Minutes match
			break;
		case MIN_MATCH:
			match_bits |= ALM0MSK0;
			this->write(alarm_reg,match_bits);   //Minutes match
			break;
		case HR_MATCH:
			match_bits |= ALM0MSK1;
			this->write(alarm_reg,match_bits);   //Hours match
			break;
		case WEEKDAY_MATCH:
			match_bits |= ALM0MSK1|ALM0MSK0;
			this->write(alarm_reg,match_bits);   //Day of week match
			break;
		case DATE_MATCH:
			match_bits |= ALM0MSK2;
			this->write(alarm_reg,match_bits);   //Date match
			break;
		case FULL_MATCH:
			match_bits |= ALM0MSK2|ALM0MSK1|ALM0MSK0;
			this->write(alarm_reg,match_bits);   //Sec, Minutes Hours, Date match
			break;
		default:
			match_bits |= ALM0MSK0;
			this->write(alarm_reg,match_bits);   //Minutes match
			break;
	}//end switch(MATCH)	
}

void MCP79410::set_mfp_function(MFPMode mode){
	unsigned char mfp_bits = this->read(ADDR_CTRL);
	switch(mode)
	{
		case GPO :              //For GPO clear SQWEN, ALM0EN, ALM1EN
			mfp_bits &= ~(SQWE|ALM_0|ALM_1);
			this->write(ADDR_CTRL,mfp_bits);
			break;
		case ALARM_INTERRUPT :  //For ALARM Interrupts clear SQWEN and set either ALM0EN or ALM1EN
			mfp_bits &= SQWE;
			mfp_bits |= ALM_0;
			this->write(ADDR_CTRL,mfp_bits);
			break;
		case SQUARE_WAVE :      //For SQUARE WAVE set SQWEN
			mfp_bits &= ~(ALM_0|ALM_1);
			mfp_bits |= SQWE;
			this->write(ADDR_CTRL,mfp_bits);
			break;
		default:                //ALARM Interrupts
			mfp_bits &= SQWE;
			mfp_bits |= ALM_0;
			this->write(ADDR_CTRL,mfp_bits);
			break;
	}//end switch(MODE)
}

uint8_t MCP79410::check_power_failure(){
	unsigned char power_bit=this->read(ADDR_DAY);
	uint8_t power_fail=0;
	if((power_bit&PWRFAIL)==PWRFAIL){
		power_fail=TRUE;
	}else{
		power_fail=FALSE;
	}
	power_bit&=~PWRFAIL;
	this->write(ADDR_DAY,power_bit);
	return power_fail;
}

uint8_t MCP79410::battery_enabled(void){
	unsigned char temp=this->read(ADDR_DAY);
	if((temp & VBATEN)==VBATEN){
		return TRUE;
	}else{
		return FALSE;
	}
}

void MCP79410::enable_battery(uint8_t enable){
	unsigned char mask=read(ADDR_DAY);
	if(enable){
		mask=(mask|VBATEN);
	}else{
		mask=(mask & VBATDIS);
	}
	write(ADDR_DAY,mask);
}

RTCC_Data MCP79410::power_up_time(void){
	this->time_data->power_up.min=bcd2dec(this->read(PWRUPMIN));
	this->time_data->power_up.hour=bcd2dec(this->read(PWRUPHOUR));	
	this->time_data->power_up.date=bcd2dec(this->read(PWRUPDATE));	
	this->time_data->power_up.month=bcd2dec(this->read(PWRUPMTH));
	RTCC_Data ret=this->time_data->power_up;
	return ret;
}

RTCC_Data MCP79410::power_down_time(void){
	this->time_data->power_down.min=bcd2dec(this->read(PWRDNMIN));
	this->time_data->power_down.hour=bcd2dec(this->read(PWRDNHOUR));
	this->time_data->power_down.date=bcd2dec(this->read(PWRDNDATE));
	this->time_data->power_down.month=bcd2dec(this->read(PWRDNMTH));
	RTCC_Data ret=this->time_data->power_down;
	return ret;
}

unsigned char MCP79410::dec2bcd(uint8_t dec){
	return ((dec/10 * 16)+(dec % 10));
}

uint8_t MCP79410::bcd2dec(unsigned char bcd){
	return ((bcd/16 *10)+(bcd % 16));
}

void MCP79410::RTCCtoTM(RTCC_Data* data,struct tm* _tm){
	_tm->tm_sec=data->sec;
	_tm->tm_min=data->min;
	_tm->tm_hour=data->hour;
	_tm->tm_mday=data->date;
	_tm->tm_wday=data->weekday+1;
	_tm->tm_mon=data->month-1;
	_tm->tm_year=((data->year+2000)-1900);
}

void MCP79410::TMtoRTCC(struct tm* _tm,RTCC_Data* rtcc_t){
	rtcc_t->sec=_tm->tm_sec;
	rtcc_t->min=_tm->tm_min;
	rtcc_t->hour=_tm->tm_hour;
	rtcc_t->date=_tm->tm_mday;
	rtcc_t->weekday=_tm->tm_wday+1;
	rtcc_t->month=_tm->tm_mon+1;
	rtcc_t->year=((_tm->tm_year+1900)-2000);
}

unsigned char MCP79410::read(uint8_t reg){
	unsigned char buffer=0;
	i2c_start_wait(ADDR_RTCC_WRITE);
	i2c_write(reg);
	//change to write mode
	i2c_rep_start(ADDR_RTCC_READ);
	buffer=i2c_readNak();
	i2c_stop();
	return buffer;
}

void MCP79410::write(unsigned char reg,unsigned char data){
	i2c_start_wait(ADDR_RTCC_WRITE);
	i2c_write(reg);
	i2c_write(data);
	i2c_stop();
}





#include <TBoard.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <MCP79410.h>
#include <AutoTimer.h>
#include <Controller.h>
#include <MilliTimer.h>
#include <Timer.h>
#include "m_general.h"
#include "m_usb.h"
//#include <USBSerial.h>
#define TESTUSB 0
#define TIME_DATE_BUFFERSIZE 12
#define DATETIMERESPONDSIZE  11
#define BUFFERSIZE TIME_DATE_BUFFERSIZE+TIME_DATE_BUFFERSIZE



typedef struct SERIAL_DATA{
	uint8_t time_count;
	uint8_t date_count;
	uint8_t buffer_count;
	char time_buffer[BUFFERSIZE];
	char date_buffer[BUFFERSIZE];
	char buffer[BUFFERSIZE+BUFFERSIZE];
	//char respondbuffer[DATETIMERESPONDSIZE];
}SERIAL_DATA;

uint8_t  EEMEM  CurrentStore;

#if TESTUSB==0

	MilliTimer milli_timer;
	ADC_POT adc_driver;
	Timer timer(&milli_timer);
	CurrentDriver currentdriver(&adc_driver);
	Controller controller(&timer,&currentdriver,&CurrentStore);

	ISR(TIMER0_OVF_vect){
		milli_timer.isr();
	}

	ISR(TIMER1_OVF_vect){
		
	}

	ISR(ADC_vect){
		
	}


	int main(void){
		controller.setup(TRUE);
		controller.start();
		controller.run();
	}

#else

	//debugging
	int main_func();
	void TestingUSB();
	int ParseDateTime(char index1,char index2);
	void SetRTC();

	int main(void){
		TestingUSB();
	}

	struct tm new_time;
	SERIAL_DATA serialData;
	MCP79410 rtc;

	void TestingUSB(){

		int size=0;
		time_t now_t;
		struct tm now_tm;
		rtc.initialize(TRUE);
		while(1){


		}
		m_usb_init();
		while(!m_usb_isconnected());
		while(1){
			bool read=false;
			if(m_usb_rx_available()){
				int count=0;
				char value=m_usb_rx_char();
				char *temp;
				uint8_t *tempCounter;
				serialData.date_count=0;
				serialData.time_count=0;
				while(value!='\r' && -1!=(int8_t)value && count<255){
					if(value=='d'){
						temp=serialData.date_buffer;
						tempCounter=&serialData.date_count;
						}else if(value=='t'){
						temp=serialData.time_buffer;
						tempCounter=&serialData.time_count;
						}else if(value=='r'){
						read=true;
					}
					if(!read){
						temp[*tempCounter]=value;
						*tempCounter+=1;
						//serialData.buffer[count]=value;
						//count+=1;
					}
					value=m_usb_rx_char();
				}
				if(read){
					now_t=rtc.Now();
					gmtime_r(&now_t,&now_tm);
					m_usb_tx_int(now_tm.tm_mon+1);
					m_usb_tx_char('-');
					m_usb_tx_int(now_tm.tm_mday);
					m_usb_tx_char('-');
					m_usb_tx_int(now_tm.tm_year+1900);
					m_usb_tx_char(' ');
				
					m_usb_tx_int(now_tm.tm_hour);
					m_usb_tx_char(':');
					m_usb_tx_int(now_tm.tm_min);
					m_usb_tx_char(':');
					m_usb_tx_int(now_tm.tm_sec);
					m_usb_tx_char('\n');
					}else{
					for(int i=0;i<serialData.date_count;i++){
						m_usb_tx_char(serialData.date_buffer[i]);
					}
					m_usb_tx_char(' ');
					for(int i=0;i<serialData.time_count;i++){
						m_usb_tx_char(serialData.time_buffer[i]);
					}
					//m_usb_tx_char('\n');
					SetRTC();
				}

			}
		}
	}

	int ParseDateTime(char index1,char index2){
		int digit1,digit2;
		digit1=index1-'0';
		digit2=index2-'0';
		if(digit1>0){
			return (digit1*10)+digit2;
			}else{
			return digit2;
		}
	}

	void SetRTC(){
		RTCC_Data new_time;
		new_time.month=ParseDateTime(serialData.date_buffer[2],serialData.date_buffer[3]);
		new_time.date=ParseDateTime(serialData.date_buffer[5],serialData.date_buffer[6]);
		new_time.year=ParseDateTime(serialData.date_buffer[8],serialData.date_buffer[9]);
	
		new_time.hour=ParseDateTime(serialData.time_buffer[2],serialData.time_buffer[3]);
		new_time.min=ParseDateTime(serialData.time_buffer[5],serialData.time_buffer[6]);
		new_time.sec=ParseDateTime(serialData.time_buffer[8],serialData.time_buffer[9]);
		rtc.set_sys_time(new_time);
	}


#endif




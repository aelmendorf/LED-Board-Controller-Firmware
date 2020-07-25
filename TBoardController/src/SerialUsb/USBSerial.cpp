/* 
* USBSerial.cpp
*
* Created: 2/7/2019 03:49:46 PM
* Author: 600076
*/


#include "USBSerial.h"

// default constructor
USBSerial::USBSerial()
{
	//Nothing to Initialize
} //USBSerial

void USBSerial::Connect(){
	while(!m_usb_isconnected());
}

void USBSerial::Initialize(){
	m_usb_init();
}//End

void USBSerial::Read(){
	if(m_usb_rx_available()){
		int counter=0;
		char value=m_usb_rx_char();
		while(value!='\r' && -1!=(uint8_t)value && counter<BUFFERSIZE){
			this->serialData.buffer[counter]=value;
			counter+=1;
			value=m_usb_rx_char();
		}
		this->serialData.buffer_count=counter;
		this->ParseBuffer();
	}
}

void USBSerial::ParseBuffer(){
	char command=this->serialData.buffer[0];
	if(command=='d'){
		if(serialData.buffer[1]==':'){
			for(int i=2;i<TIME_DATE_BUFFERSIZE;i++){
				this->serialData.date_buffer[i]=this->serialData.buffer[i];
			}
		}
	}else if(command=='t'){
		
	}
}

void USBSerial::Respond(){
	for(int i=0;i<TIME_DATE_BUFFERSIZE;i++){
		m_usb_tx_char(this->serialData.date_buffer[i]);
	}
}

// default destructor
USBSerial::~USBSerial()
{
} //~USBSerial

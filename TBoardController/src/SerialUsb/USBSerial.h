
#ifndef __USBSERIAL_H__
#define __USBSERIAL_H__
#include "m_general.h"
#include "m_usb.h"

#define TIME_DATE_BUFFERSIZE 12
#define BUFFERSIZE TIME_DATE_BUFFERSIZE+TIME_DATE_BUFFERSIZE


typedef struct SERIAL_DATA{
	uint8_t time_count;
	uint8_t date_count;
	uint8_t buffer_count;
	char time_buffer[BUFFERSIZE];
	char date_buffer[BUFFERSIZE];
	char buffer[BUFFERSIZE+BUFFERSIZE];
}SERIAL_DATA;

class USBSerial
{
public:

protected:
private:
	SERIAL_DATA serialData;
//functions
public:
	void Initialize();
	void Connect();
	void ParseBuffer();
	USBSerial();
	void Read();
	void Respond();
	~USBSerial();
protected:
private:
	
	USBSerial(const USBSerial &c);
	USBSerial& operator=( const USBSerial &c );

}; //USBSerial

#endif //__USBSERIAL_H__

/*
*
* Created: 9/7/2018
* Author: Andrew Elmendorf
* Company: Sensor Electronic Technology
* Description:
* Simple 2wire/i2c/SPL-SCL driver
*
* ToDo:
*   Define MCP79410(RTC).  Will likely make generic "Device" class and add it to i2c class
*	https://www.microchip.com/wwwproducts/en/MCP79410
*
*
*/


#ifndef __I2C_H__
#define __I2C_H__

#include <TBoard.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <math.h>
#include <binary_search.h>

typedef enum i2c_events{
	SUCCESS,
	SUCCESS_WRITE,
	SUCCESS_READ,
	ERROR_BUS_COLLISION,
	ERROR_WRITE_BYTE_ADDRESS,
	ERROR_WRITE_ADDRESS,
	ERROR_WRITE_BYTE,
	ERROR_WRITE_DATA,
	ERROR_MAX_RETRIES,
	ERROR_NULL_DATA,
	ERROR_COMPLETE_FAILURE
}I2C_EVENTS;


class I2C
{
	//variables
	public:
	protected:
	private:
		uint8_t address;
		uint8_t byte_address;
	//functions
	public:
		I2C(uint8_t address,uint8_t baddr);
		void init(uint8_t address,uint8_t baddr);
		I2C_EVENTS start();
		I2C_EVENTS write_byte(uint8_t data);
		I2C_EVENTS write(uint8_t* data,uint8_t number_bytes);
		I2C_EVENTS read(uint8_t* data,uint8_t number_bytes);
		uint8_t read_byte_ACK(void);
		uint8_t read_byte_NACT(void);
		void stop(void);
		~I2C();
	protected:
	private:
}; //i2c

#endif //__I2C_H__

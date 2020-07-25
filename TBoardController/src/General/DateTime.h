/* 
* DateTime.h
*
* Created: 9/25/2018 9:22:37 AM
* Author: 600076
*/


#ifndef __DATETIME_H__
#define __DATETIME_H__
#include <TBoard.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <math.h>
#include <Helpers.h>

class DateTime
{
//variables
public:
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
protected:
private:

//functions
public:
	DateTime();
	DateTime(const DateTime &c);
	DateTime& operator=(const DateTime &c);
	uint32_t operator-(const DateTime &rhs)const;
	uint8_t operator>(const DateTime &rhs)const;
	uint8_t operator<(const DateTime &rhs)const;
	uint8_t operator==(const DateTime &rhs)const;
~DateTime();
protected:
private:



}; //DateTime

#endif //__DATETIME_H__

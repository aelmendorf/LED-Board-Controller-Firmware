/* 
* DateTime.cpp
*
* Created: 9/25/2018 9:22:37 AM
* Author: 600076
*/


#include "DateTime.h"

// default constructor
DateTime::DateTime()
{
	this->year=0;
	this->month=0;
	this->day=0;
	this->hour=0;
	this->min=0;
	this->sec=0;
} //DateTime

// default destructor
DateTime::~DateTime()
{
	
} //~DateTime

DateTime::DateTime(const DateTime &copy){
	this->year=copy.year;
	this->month=copy.month;
	this->day=copy.day;
	this->hour=copy.hour;
	this->min=copy.min;
	this->sec=copy.sec;
}

//rhs must be < lhs
uint32_t DateTime::operator-(const DateTime &rhs)const{
	uint32_t ret=(this->sec-rhs.sec)+(this->min-rhs.min)*60 + (this->hour-rhs.hour)*3600;
	/*if(this->day>rhs.day){
		return 0xFFFF-ret;
	}else{
		return ret;
	}*/
	return ret;
}

DateTime& DateTime::operator =(const DateTime &copy){
	this->year=copy.year;
	this->month=copy.month;
	this->day=copy.day;
	this->hour=copy.hour;
	this->min=copy.min;
	this->sec=copy.sec;
	return *this;
}

uint8_t DateTime::operator <(const DateTime &rhs)const{
	if(this->year<rhs.year)
		return 1;
	
	if(this->year>rhs.year)
		return 0;
	
	if(this->month<rhs.month)
		return 1;
	
	if(this->month>rhs.month)
		return 0;
	
	if(this->day<rhs.day)
		return 1;
	
	if(this->day>rhs.day)
		return 0;

	if(this->hour<rhs.hour)
		return 1;
	
	if(this->hour>rhs.hour)
		return 0;
	
	if(this->min<rhs.min)
		return 1;
	
	if(this->min>rhs.min)
		return 0;
	

	if(this->sec<rhs.sec)
		return 1;
	
	if(this->sec>rhs.sec)
		return 0;

	return 0;
}

uint8_t DateTime::operator >(const DateTime &rhs)const{
	if(this->year<rhs.year)
		return 0;
	
	if(this->year>rhs.year)
		return 1;
		
	
	if(this->month<rhs.month)
		return 0;
	
	if(this->month>rhs.month)
		return 1;
		
	if(this->day<rhs.day)
		return 0;	
	
	if(this->day>rhs.day)
		return 1;


	if(this->hour<rhs.hour)
		return 0;
		
	if(this->hour>rhs.hour)
		return 1;
	
	if(this->min<rhs.min)
		return 0;
	
	if(this->min>rhs.min)
		return 1;
		

	if(this->sec<rhs.sec)
		return 0;
		
	if(this->sec>rhs.sec)
		return 1;

	return 0;
}

uint8_t DateTime::operator ==(const DateTime &rhs)const{
	return (this->year==rhs.year) && (this->month==rhs.month) && (this->day==rhs.day) && (this->hour==rhs.hour) &&
			(this->min==rhs.min) && (this->sec==rhs.sec);
}
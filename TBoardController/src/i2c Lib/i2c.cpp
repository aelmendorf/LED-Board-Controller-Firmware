/* 
* i2c.cpp
*
* Created: 9/7/2018
* Author: Andrew Elmendorf
* Company: Sensor Electronic Technology
* Description:
* Simple 2wire/i2c/SPL-SCL driver.
* 
* ToDo:
*   Define MCP79410(RTC).  Will likely make generic "Device" class and add it to i2c class
*	have most definitions in testing project.  need to double check some addressing..
*	https://www.microchip.com/wwwproducts/en/MCP79410
*
*/
#include "i2c.h"

I2C::I2C(uint8_t address,uint8_t baddr)
{
	this->init(address,baddr);
}

void I2C::init(uint8_t address,uint8_t baddr){
	this->address=address;
	this->byte_address=baddr;
	
	DDRC&=~(1<<SDA)|~(1<<SCL);
	PORTC&=~(1<<SDA)|~(1<<SCL);
	TWSR=0;//clear prescaler
	
	#if F_CPU <1600000UL
		TWBR=0;
	#else
		TWBR=((F_CPU/400000UL)-16)/2;
	#endif
	TWCR|=(1<<TWEN);
}

I2C_EVENTS I2C::start(){
	TWCR =(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	//wait for TWINT flag, start transmitted
	while (!(TWCR &(1<<TWINT)));
	
	//check status (0xf8) is error mask
	switch(TW_STATUS){
		case TW_START:
		case TW_REP_START:
			return SUCCESS;
		default:
		return ERROR_BUS_COLLISION;
	}
}

void I2C::stop(void){
	TWCR|=(1<<TWINT) |(1<<TWSTO) | (1<<TWEN);
	//wait for TWSTO flag, stop transmitted
	while(TWCR & (1<<TWSTO));
}//End stop

I2C_EVENTS I2C::write_byte(uint8_t data){
	TWDR=data;
	TWCR=(1<<TWINT)|(1<<TWEN);
	while(!(TWCR&(1<<TWINT)));
	switch(TW_STATUS){
		case TW_MT_SLA_ACK:    
		case TW_MT_DATA_ACK:
		case TW_MR_SLA_ACK:
		case TW_MR_DATA_ACK:
		return SUCCESS;//Success
		        
		case TW_BUS_ERROR:      
		case TW_MT_SLA_NACK:
		case TW_MT_DATA_NACK:
		case TW_MR_SLA_NACK:
		case TW_MR_DATA_NACK:
		case TW_MT_ARB_LOST:
		return ERROR_WRITE_BYTE; //Error
		        
		default:
		return ERROR_COMPLETE_FAILURE;           //complete failure
	}
}
/************************************************************************/
/* Read byte in master mode                                             */
 /* 1.issue start														*/
 /*2.try to write slave address											*/
/* 3.Put the byte address that u want to write to on the bus			*/
/* 4.put a data byte onto the bus										*/
/* 5.repeat step 4 for all bytes										*/
/* 6.issue a stop condition												*/
/*************************************************************************/
I2C_EVENTS I2C::write(uint8_t* data,uint8_t number_bytes){
	uint8_t restarts=MAX_RESTARTS;
	uint8_t retry=1;
	if(number_bytes==0){
		return ERROR_NULL_DATA;
	}//End check #bytes
	while(retry){
		retry=0;
		if(this->start()){
			if((restarts--)==0){
				return ERROR_MAX_RETRIES;
			}else{
				retry=1;
			}//End retry/error handle
		}else{
			if(this->write_byte(this->address<<1|TW_WRITE)){
				if(--restarts==0){
					this->stop();
					return ERROR_WRITE_ADDRESS;
				}else{
					retry=ERROR_MAX_RETRIES;
				}//End retry/error handle
			}else{
				//write out slave address
				if(this->write_byte(this->byte_address)){
					return ERROR_WRITE_BYTE_ADDRESS;
				}//End
				
				//write out the buffer
				for(;number_bytes>0;number_bytes--){
					if(this->write_byte(*data++)){
						return ERROR_WRITE_DATA;
					}//End error handle
				}//End write out buffer
				
				//We did it!! finally!!
				retry=0;
				this->stop();
				return SUCCESS_WRITE;
			}//End error check
		}//End main write sequence
	}//End while loop 
}//End write

/************************************************************************/
/* Read byte in master mode                                             */
/* 1.issue start														*/
/*2.try to write slave address	
/* if busy NACT returned
	continue until ACT returned or 
	out of retires														*/
/*************************************************************************/
I2C_EVENTS I2C::read(uint8_t* data,uint8_t number_bytes){
	uint8_t restarts=MAX_RESTARTS;
	uint8_t retry=1;
	if(number_bytes==0){
		return ERROR_NULL_DATA;
	}//End check #bytes
	while(retry){
		retry=0;
		if(this->start()){
			if((restarts--)==0){
				return ERROR_MAX_RETRIES;
				}else{
				retry=1;
			}//End retry/error handle
			}else{
				if(this->write_byte(this->address<<1|TW_WRITE)){
					if(--restarts==0){
						this->stop();
						return ERROR_MAX_RETRIES;
					}else{
						retry=1;
					}//End retry/error handle
				}else{
					//write out slave address
					if(this->write_byte(this->byte_address)){
						return ERROR_WRITE_BYTE_ADDRESS;
					}//End
				
					if(this->start()){
						return ERROR_COMPLETE_FAILURE;
					}
					if(this->write_byte(this->address<<1|TW_READ)){
						return ERROR_WRITE_ADDRESS;
					}
					
					//receive bytes and put in buffer
					for(;number_bytes>1;number_bytes--){
						*(data++)=this->read_byte_ACK();//write back all but last.  last is NACK
					}//end
					
					*(data++)=this->read_byte_NACT();
					//We did it!! finally!!
					retry=0;
					this->stop();
					return SUCCESS_READ;
			}//End error check
		}//End main write sequence
	}//End while loop
}//End write

/************************************************************************/
/* Read Acknowledge(ACK)                                                */
/************************************************************************/
uint8_t I2C::read_byte_ACK(){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	
	return TWDR;
}//End read byte slave

/************************************************************************/
/* Read Not Acknowledge(NACT)                                      */
/************************************************************************/
uint8_t I2C::read_byte_NACT(){
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}//End read byte master


I2C::~I2C()
{
	//nothing to cleanup yet
} //~i2c

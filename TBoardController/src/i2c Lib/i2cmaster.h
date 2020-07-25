#ifndef _I2CMASTER_H
#define _I2CMASTER_H

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
#define F_CPU 16000000UL
#include <avr/io.h>
#include <inttypes.h>
#include <util/twi.h>

#ifndef F_CPU
#define F_CPU 4000000UL
#endif

#define SCL_CLOCK  100000L
#define I2C_READ    1
#define I2C_WRITE   0
#define SDA	PORTD1
#define SCL	PORTD0

inline void i2c_init(void){
	DDRD&=~(1<<SDA)|~(1<<SCL);
	PORTD&=~(1<<SDA)|~(1<<SCL);
	TWSR = 0;                        
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2;
}

inline void i2c_stop(void){
	/* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	  
	// wait until stop condition is executed and bus released
	while(TWCR & (1<<TWSTO));
}

inline unsigned char i2c_start(unsigned char addr){
	    uint8_t   twst;

	    // send START condition
	    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	    // wait until transmission completed
	    while(!(TWCR & (1<<TWINT)));

	    // check value of TWI Status Register. Mask prescaler bits.
	    twst = TW_STATUS & 0xF8;
	    if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;

	    // send device address
	    TWDR = addr;
	    TWCR = (1<<TWINT) | (1<<TWEN);

	    // wail until transmission completed and ACK/NACK has been received
	    while(!(TWCR & (1<<TWINT)));

	    // check value of TWI Status Register. Mask prescaler bits.
	    twst = TW_STATUS & 0xF8;
	    if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;

	    return 0;
}

inline unsigned char i2c_rep_start(unsigned char addr){
	return i2c_start(addr);
}

inline void i2c_start_wait(unsigned char addr){
	uint8_t   twst;
	while (1)
	{
		// send START condition
		TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
		    
		// wait until transmission completed
		while(!(TWCR & (1<<TWINT)));
		    
		// check value of TWI Status Register. Mask prescaler bits.
		twst = TW_STATUS & 0xF8;
		if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
		    
		// send device address
		TWDR = addr;
		TWCR = (1<<TWINT) | (1<<TWEN);
		    
		// wail until transmission completed
		while(!(TWCR & (1<<TWINT)));
		    
		// check value of TWI Status Register. Mask prescaler bits.
		twst = TW_STATUS & 0xF8;
		if ((twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK))
		{
			/* device busy, send stop condition to terminate write operation */
			TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
			    
			// wait until stop condition is executed and bus released
			while(TWCR & (1<<TWSTO));
			    
			continue;
		}
		//if( twst != TW_MT_SLA_ACK) return 1;
		break;
	}
}

inline unsigned char i2c_write(unsigned char data){
    uint8_t   twst;
    
    // send data to the previously addressed device
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait until transmission completed
    while(!(TWCR & (1<<TWINT)));

    // check value of TWI Status Register. Mask prescaler bits
    twst = TW_STATUS & 0xF8;
    if( twst != TW_MT_DATA_ACK) return 1;
    return 0;
}

inline unsigned char i2c_readAck(void){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));

	return TWDR;
}

inline unsigned char i2c_readNak(void){
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	return TWDR;	
}

inline unsigned char i2c_read(unsigned char ack);
#define i2c_read(ack)  (ack) ? i2c_readAck() : i2c_readNak(); 

#endif

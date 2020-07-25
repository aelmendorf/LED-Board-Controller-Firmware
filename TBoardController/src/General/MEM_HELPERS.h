/*
 * MEM_HELPERS.h
 *
 * Created: 9/11/2018 10:40:23 AM
 *  Author: 600076
 */ 


#ifndef MEM_HELPERS_H_
#define MEM_HELPERS_H_


#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))


#define bitIsHigh(address, bit)		(address & (1 << bit))
#define bitIsLow(address, bit)		(address & (1 << bit)) == 0

#define setHigh(address, bit)		address |= 1 << bit
#define setLow(address, bit)		address &= ~(1 << bit)

#define  waitHigh(address, bit)	while (bitIsLow(address, bit));
#define  waitLow(address, bit)	while (bitIsHigh(address, bit));

#define  swapBit(address, bit)	address ^= (1<<(bit))



#endif /* MEM_HELPERS_H_ */
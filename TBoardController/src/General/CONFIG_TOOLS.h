/*
 * CONFIG_TOOLS.h
 *
 * Created: 9/11/2018 10:43:48 AM
 *  Author: 600076
 */ 
#pragma once

#ifndef CONFIG_TOOLS_H_
#define CONFIG_TOOLS_H_
#include <stdbool.h>
// typedef enum pin_mode{
// 	INPUT,
// 	OUTPUT
// }PIN_MODE;
// 
// typedef enum pin_level{
// 	HIGH,
// 	LOW	
// }PIN_LEVEL;
// 
// 
// static void portPinMode(volatile uint8_t *control,volatile uint8_t *port,const uint8_t &pin,const PIN_MODE &mode,const PIN_LEVEL &level,const bool enablePullups){	
// 	switch(mode){
// 		case INPUT:{
// 			*control&=~(1<<pin);
// 			if(enablePullups){
// 				*port|=(1<<pin);
// 			}
// 			break;
// 		}
// 		case OUTPUT:{
// 			*control|=(1<<pin);
// 			switch(level){
// 				case HIGH:{
// 					*port|=(1<<pin);
// 					break;
// 				}
// 				case LOW:{
// 					*port&=~(1<<pin);
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 	}
// }

#endif /* CONFIG_TOOLS_H_ */
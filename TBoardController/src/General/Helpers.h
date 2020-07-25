#ifndef HELPERS_H_
#define HELPERS_H_

#include <TBoard.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <time.h>

inline uint32_t map(uint32_t x,uint32_t in_min,uint32_t in_max,uint32_t out_min,uint32_t out_max)
{
	return (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}

inline int BinarySearch(int arr[],int Value,char *fraction,int l)
{
	int first, last, middle;
	first = 0;
	last = l-1;
	middle = (first+last)/2;
	
	while(first <= last)
	{
		if (arr[middle] < Value){
			first = middle + 1;
		}else if ( arr[middle] == Value ){
			*fraction = 0;  // Value Matched
			return middle;
		}else{
			last = middle - 1;
		}
		middle = (first + last)/2;
	}
	if( first > last ){
		int diff = arr[middle+1]-arr[middle];
		*fraction = (Value-arr[middle])*10/diff;
	}
	return middle;
}

inline uint8_t fromBCD(uint8_t byte)
{
	return ((byte & 0xF0) >> 4) * 10 + (byte & 0xF);
}

inline uint8_t toBCD(uint8_t byte)
{
	return (((byte / 10) % 10) << 4) + (byte % 10);
}

// inline void fromUTC(struct tm *timeptr,time_t *time_sec){
// 	struct tm start_dlt,stop_dlt;
// 	time_t start_t,stop_t;
// 	*time_sec=mk_gmtime(timeptr);
// 	*time_sec+=(Timezone*ONE_HOUR);
// 	if(Timezone==EST){
// 		//2018 start
// 		start_dlt.tm_sec=0;
// 		start_dlt.tm_min=0;
// 		start_dlt.tm_hour=DLT_HOUR;
// 		start_dlt.tm_mon=DLT_START_MONTH-TM_MONTH_OFFSET;
// 		start_dlt.tm_year=timeptr->tm_year;
// 		
// 		stop_dlt.tm_sec=0;
// 		stop_dlt.tm_min=0;
// 		stop_dlt.tm_hour=DLT_HOUR;
// 		stop_dlt.tm_mon=DLT_END_MONTH-TM_MONTH_OFFSET;
// 		stop_dlt.tm_year=timeptr->tm_year;
// 		
// 		int year=timeptr->tm_year+TM_YEAR_OFFSET;
// 		switch(year){
// 			case 2018:{
// 				start_dlt.tm_mday=DLT_2018_START_DATE;
// 				stop_dlt.tm_mday=DLT_2018_END_DATE;
// 				start_t=mk_gmtime(&start_dlt);
// 				stop_t=mk_gmtime(&stop_dlt);
// 				break;
// 			}
// 			case 2019:{
// 				start_dlt.tm_mday=DLT_2019_START_DATE;
// 				stop_dlt.tm_mday=DLT_2019_END_DATE;
// 				start_t=mk_gmtime(&start_dlt);
// 				stop_t=mk_gmtime(&stop_dlt);
// 				break;
// 			}
// 			case 2021:{
// 				start_dlt.tm_mday=DLT_2020_START_DATE;
// 				stop_dlt.tm_mday=DLT_2020_END_DATE;
// 				start_t=mk_gmtime(&start_dlt);
// 				stop_t=mk_gmtime(&stop_dlt);
// 				break;
// 			}
// 		}
// 		
// 		if(*time_sec>start_t  && *time_sec<stop_t){
// 			*time_sec+=ONE_HOUR;
// 		}else{
// 			*time_sec-=ONE_HOUR;
// 		}
// 		timeptr=gmtime(time_sec);
// 	}
// }
// 
// inline void toUTC(struct tm *timeptr,time_t *time_sec){
// 	struct tm start_dlt,stop_dlt;
// 	time_t start_t,stop_t;
// 	uint8_t is_dst=0;
// 	*time_sec=mk_gmtime(timeptr);
// 	*time_sec+=(Timezone*ONE_HOUR);
// 	if(Timezone==EST){
// 		//2018 start
// 		start_dlt.tm_sec=0;
// 		start_dlt.tm_min=0;
// 		start_dlt.tm_hour=DLT_HOUR;
// 		start_dlt.tm_mon=DLT_START_MONTH-TM_MONTH_OFFSET;
// 		start_dlt.tm_year=timeptr->tm_year;
// 					
// 		stop_dlt.tm_sec=0;
// 		stop_dlt.tm_min=0;
// 		stop_dlt.tm_hour=DLT_HOUR;
// 		stop_dlt.tm_mon=DLT_END_MONTH-TM_MONTH_OFFSET;
// 		stop_dlt.tm_year=timeptr->tm_year;
// 		int year=timeptr->tm_year+TM_YEAR_OFFSET;
// 		switch(year){
// 			case 2018:{
// 				start_dlt.tm_mday=DLT_2018_START_DATE;
// 				stop_dlt.tm_mday=DLT_2018_END_DATE;
// 				start_t=mk_gmtime(&start_dlt);
// 				stop_t=mk_gmtime(&stop_dlt);
// 				break;
// 			}
// 			case 2019:{
// 				start_dlt.tm_mday=DLT_2019_START_DATE;
// 				stop_dlt.tm_mday=DLT_2019_END_DATE;
// 				start_t=mk_gmtime(&start_dlt);
// 				stop_t=mk_gmtime(&stop_dlt);
// 				break;
// 			}
// 			case 2021:{
// 				start_dlt.tm_mday=DLT_2020_START_DATE;
// 				stop_dlt.tm_mday=DLT_2020_END_DATE;
// 				start_t=mk_gmtime(&start_dlt);
// 				stop_t=mk_gmtime(&stop_dlt);
// 				break;
// 			}
// 		}	
// 	}
// 	
// 	if(*time_sec>start_t  && *time_sec<stop_t){
// 		is_dst=1;
// 	}else{
// 		is_dst=0;
// 	}
// 	gmtime_r(;
// }


#endif /* HELPERS_H_ */
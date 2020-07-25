/*
 * TBoard.h
 *
 * Created: 9/7/2018 8:38:51 AM
 *  Author: 600076
 */ 


#ifndef TBOARD_H_
#define TBOARD_H_

#define F_CPU 16000000UL
#define CUSTOMER	 0
#define INHOUSE		 1

#define VERSION_1    0
#define VERSION_2    1

#define USE_MODE	CUSTOMER
#define VERSION		VERSION_2
//#define SET_TIME TRUE
#define SET_TIME FALSE

#define MST (-7)
#define UTC (0)
#define CCT (+8)
#define EST (-5)
#define KRN (+9)

#define Timezone EST
#define DaylightSavings 1  //1 adjust 0: don't adjust

#define NUM_AUTOTIMES 2


#define Auto1Duration		3*ONE_HOUR
#define Auto2Duration		6*ONE_HOUR
#define AUTO_HR				21
#define AUTO_MIN			0

#define ON_CURRENT			7.5
#define AUTO_CURRENT		15

#if VERSION==VERSION_1
#define AUTO_DURATION		3*ONE_HOUR
#elif VERSION==VERSION_2
#define AUTO_DURATION		6*ONE_HOUR
#endif


#define SET_MIN         0x47
#define SET_HOUR        0x14
#define SET_DATE        0x03
#define SET_MONTH       0x12
#define SET_YEAR        0x18
#define TM_MONTH_OFFSET 1
#define TM_YEAR_OFFSET  1900

#define DLT_HOUR  2
#define DLT_END_MONTH 11
#define DLT_START_MONTH 3

#define DLT_2018_START_DATE  11
#define DLT_2018_END_DATE    4
#define DLT_2019_START_DATE  DLT_2018_START_DATE-1
#define DLT_2019_END_DATE    DLT_2018_END_DATE-1
#define DLT_2020_START_DATE  DLT_2019_START_DATE-1
#define DLT_2020_END_DATE    DLT_2019_END_DATE-1


/************************************************************************/
/*                                                                      */
/************************************************************************/
#define MAX_CURRENT 20
#define CURRENT_DRIVER PORTC7
#define POT PORTF1
#define CURRENT_1 5
#define CURRENT_2 10
#define CURRENT_3 15
#define CURRENT_4 20

/************************************************************************/
/* ADC                                                                  */
/************************************************************************/
#define MAX_ADC 1023

/************************************************************************/
/* Controller                                                           */
/************************************************************************/
#define ON_PWM			900
#define AUTO_PWM		900
#define ON_PWM_IDLE		1000
#define AUTO_PWM_IDLE	1000
#define PWM_OFF			1023
#define LEDSEQCOUNT		6
#define LEDSEQSTEPS		4
#define RED_POS			0
#define GREEN_POS		1
#define BLUE_POS		3
#define ORANGE_POS		2
#define RED				PORTB4
#define BLUE			PORTB6
#define GREEN			PORTB5
#define POS1			PIND5
#define POS2			PIND3
#define POS3			PIND2
#define LID_SWITCH		PINF5
#define POS1_B			0
#define POS2_B			1
#define POS3_B			2
#define LID_B			3

#define POT				PORTF1
#define VBAT			PORTF0
#define PROGRAM_SEQ		0
#define M_ON		    0
#define M_OFF			2
#define M_AUTO			1
#define M_PROGRAM		3

#define START_T1_HR		4
#define START_T1_MIN	32
#define START_T1_DUR	1
#define START_T2		5
#define AUTO_COUNT		1
#define AUTO_A			0

#define LATCH_PERIOD	500
#define CHECK_PERIOD	5000/*3000000*/
#define SLEEP_PERIOD	300000UL
#define AUTO_BLINK		5000

#define HRS_TO_SEC		3600
#define MIN_TO_SEC		60

#define DEFAULT_CURRENT 10

#define PROGRAM_MODE_ENABLED   0
#define PROGRAM_VERSION        0 

/************************************************************************/
/* i2c                                                                  */
/************************************************************************/

#define MAX_RESTARTS				20

#define TW_START					0x08
#define TW_REP_START				0x10
// Master Transmitter
#define TW_MT_SLA_ACK				0x18
#define TW_MT_SLA_NACK				0x20
#define TW_MT_DATA_ACK				0x28
#define TW_MT_DATA_NACK				0x30
#define TW_MT_ARB_LOST				0x38
// Master Receiver
#define TW_MR_ARB_LOST				0x38
#define TW_MR_SLA_ACK				0x40
#define TW_MR_SLA_NACK				0x48
#define TW_MR_DATA_ACK				0x50
#define TW_MR_DATA_NACK				0x58
#define TW_BUS_ERROR				0x00
#define TWSR_STATUS_MASK            0xF8
/*#define TW_STATUS                   TWSR & TWSR_STATUS_MASK*/
//read or write modes
#define TW_READ                     1
#define TW_WRITE                    0

/************************************************************************/
/* MilliTimer                                                           */
/************************************************************************/

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                   GLOBAL CONSTANTS RTCC - REGISTERS ADDRESSES
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define  ADDR_EEPROM_WRITE 0xAE       //  DEVICE ADDR for EEPROM (writes)
#define  ADDR_EEPROM_READ  0xAF       //  DEVICE ADDR for EEPROM (reads)
#define  ADDR_RTCC_WRITE   0xDE       //  DEVICE ADDR for RTCC MCHP  (writes)
#define  ADDR_RTCC_READ    0xDF       //  DEVICE ADDR for RTCC MCHP  (reads)
//.............................................................................................
#define  SRAM_PTR          0x20       //  pointer of the SRAM area (RTCC)
#define  ADDR_EEPROM_SR    0xFF       //  STATUS REGISTER in the  EEPROM
//.............................................................................................
#define  ADDR_SEC          0x00       //  address of SECONDS      register
#define  ADDR_MIN          0x01       //  address of MINUTES      register
#define  ADDR_HOUR         0x02       //  address of HOURS        register
#define  ADDR_DAY          0x03       //  address of DAY OF WK    register
#define  ADDR_STAT         0x03       //  address of STATUS       register
#define  ADDR_DATE         0x04       //  address of DATE         register
#define  ADDR_MNTH         0x05       //  address of MONTH        register
#define  ADDR_YEAR         0x06       //  address of YEAR         register
#define  ADDR_CTRL         0x07       //  address of CONTROL      register
#define  ADDR_CAL          0x08       //  address of CALIB        register
#define  ADDR_ULID         0x09       //  address of UNLOCK ID    register
//............................................................................................
#define  ADDR_ALM0SEC      0x0a       //  address of ALARM0 SEC   register
#define  ADDR_ALM0MIN      0x0b       //  address of ALARM0 MIN   register
#define  ADDR_ALM0HR       0x0c       //  address of ALARM0 HOUR  register
#define  ADDR_ALM0CTL      0x0d       //  address of ALARM0 CONTR register
#define  ADDR_ALM0DAT      0x0e       //  address of ALARM0 DATE  register
#define  ADDR_ALM0MTH      0x0f       //  address of ALARM0 MONTH register
//.............................................................................................
#define  ADDR_ALM1SEC      0x11       //  address of ALARM1 SEC   register
#define  ADDR_ALM1MIN      0x12       //  address of ALARM1 MIN   register
#define  ADDR_ALM1HR       0x13       //  address of ALARM1 HOUR  register
#define  ADDR_ALM1CTL      0x14       //  address of ALARM1 CONTR register
#define  ADDR_ALM1DAT      0x15       //  address of ALARM1 DATE  register
#define  ADDR_ALM1MTH      0x16       //  address of ALARM1 MONTH register
//.............................................................................................
#define  PWRDNMIN          0x18       //  address of T_SAVER MIN(VDD->BAT)
#define  PWRDNHOUR         0x19       //  address of T_SAVER HR (VDD->BAT)
#define  PWRDNDATE         0x1a       //  address of T_SAVER DAT(VDD->BAT)
#define  PWRDNMTH          0x1b       //  address of T_SAVER MTH(VDD->BAT)
//.............................................................................................
#define  PWRUPMIN          0x1c       //  address of T_SAVER MIN(BAT->VDD)
#define  PWRUPHOUR         0x1d       //  address of T_SAVER HR (BAT->VDD)
#define  PWRUPDATE         0x1e       //  address of T_SAVER DAT(BAT->VDD)
#define  PWRUPMTH          0x1f       //  address of T_SAVER MTH(BAT->VDD)
//.............................................................................................

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                  GLOBAL CONSTANTS RTCC - INITIALIZATION
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define  PM                0x20       //  post-meridian bit (ADDR_HOUR)
#define  OUT_PIN           0x80       //  = b7 (ADDR_CTRL)
#define  SQWE              0x40       //  SQWE = b6 (ADDR_CTRL)
#define  ALM_NO            0x00       //  no alarm activated        (ADDR_CTRL)
#define  ALM_0             0x10       //  ALARM0 is       activated (ADDR_CTRL)
#define  ALM_1             0x20       //  ALARM1 is       activated (ADDR_CTRL)
#define  ALM_01            0x30       //  both alarms are activated (ADDR_CTRL)
#define  MFP_01H           0x00       //  MFP = SQVAW(01 HERZ)      (ADDR_CTRL)
#define  MFP_04K           0x01       //  MFP = SQVAW(04 KHZ)       (ADDR_CTRL)
#define  MFP_08K           0x02       //  MFP = SQVAW(08 KHZ)       (ADDR_CTRL)
#define  MFP_32K           0x03       //  MFP = SQVAW(32 KHZ)       (ADDR_CTRL)
#define  MFP_64H           0x04       //  MFP = SQVAW(64 HERZ)      (ADDR_CTRL)
#define  ALMx_POL          0x80       //  polarity of MFP on alarm  (ADDR_ALMxCTL)
#define  ALMxC_SEC         0x00       //  ALARM compare on SEC      (ADDR_ALMxCTL)
#define  ALMxC_MIN         0x10       //  ALARM compare on MIN      (ADDR_ALMxCTL)
#define  ALMxC_HR          0x20       //  ALARM compare on HOUR     (ADDR_ALMxCTL)
#define  ALMxC_DAY         0x30       //  ALARM compare on DAY      (ADDR_ALMxCTL)
#define  ALMxC_DAT         0x40       //  ALARM compare on DATE     (ADDR_ALMxCTL)
#define  ALMxC_ALL         0x70       //  ALARM compare on all param(ADDR_ALMxCTL)
#define  ALMx_IF           0x08       //  MASK of the ALARM_IF      (ADDR_ALMxCTL)
#define  OSCON             0x20       //  state of the oscillator(running or not)
#define  EXTOSC			   0x08		  //external OSC
#define  VBATEN            0x08       //  enable battery for back-up
#define	 VBATDIS		   0x37       // disabe battery for back-up
#define  PWRFAIL	       0x10
//#define  SQVAW    PORTAbits.RA4      //square wave MFP from RTCC
#define  START_32KHZ       0x80       //start crystal: ST = b7 (ADDR_SEC)
#define  LP                0x20       //  mask for the leap year bit(MONTH REG)
#define  HOUR_12           0x40       //  12 hours format   (ADDR_HOUR)

#define ALM1MSK2           0x40
#define ALM1MSK1           0x20
#define ALM1MSK0           0x10
#define ALM0MSK2           0x40
#define ALM0MSK1           0x20
#define ALM0MSK0           0x10

#define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year
#define  CalendarYrToTm(Y)   ((Y) - 1970)
#define  tmYearToY2k(Y)      ((Y) - 30)    // offset is from 2000
#define  y2kYearToTm(Y)      ((Y) + 30)

#define SECS_PER_MIN  ((time_t)(60UL))
#define SECS_PER_HOUR ((time_t)(3600UL))
#define SECS_PER_DAY  ((time_t)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((time_t)(7UL))
#define SECS_PER_WEEK ((time_t)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR ((time_t)(SECS_PER_DAY * 365UL)) 
#define SECS_YR_2000  ((time_t)(946684800UL)) // the time at the start of y2k

#define minutesToTime_t ((M)) ( (M) * SECS_PER_MIN)
#define hoursToTime_t   ((H)) ( (H) * SECS_PER_HOUR)
#define daysToTime_t    ((D)) ( (D) * SECS_PER_DAY)
#define weeksToTime_t   ((W)) ( (W) * SECS_PER_WEEK)

#define bcdtodec_m (bcd)((((bcd) & 0xF0)>>4)*10+((bcd) & 0xF))
#define dectobcd_m (dec)( ( (((dec)/10) % 10) << 4) + ((dec) % 10) )


#define TRUE 1
#define FALSE 0

#define HIGH  1
#define LOW   0

#endif /* TBOARD_H_ */


/*
 * dot_display.h
 *
 * Created: 03.01.2017 16:16:46
 *  Author: Oleksandr
 */ 


#ifndef DOT_DISPLAY_H_
#define DOT_DISPLAY_H_

#include <stdlib.h>
#include <util/atomic.h>
#include "common.h"

#define DISPLAY_ROW_N       3
#define DISPLAY_COLUMN_N    10
#define DISPLAY_MIN_O_COLUMN_START     0
#define DISPLAY_MIN_O_COLUMN_END       2
#define DISPLAY_MIN_T_COLUMN_START     3
#define DISPLAY_MIN_T_COLUMN_END       4
#define DISPLAY_HOUR_O_COLUMN_START    5
#define DISPLAY_HOUR_O_COLUMN_END      7
#define DISPLAY_HOUR_T_COLUMN_START    8
#define DISPLAY_HOUR_T_COLUMN_END      9

#define DISPLAY_MIN_O_MASK			(0x0007)
#define DISPLAY_MIN_T_MASK			(0x0018)
#define DISPLAY_HOUR_O_MASK			(0x00E0)
#define DISPLAY_HOUR_T_MASK			(0x0300)

#define DISPLAY_MIN_O_ROW_WIDTH			3
#define DISPLAY_MIN_T_ROW_WIDTH			2
#define DISPLAY_HOUR_O_ROW_WIDTH		3
#define DISPLAY_HOUR_T_ROW_WIDTH		2

#define DISPLAY_MIN_O_N_MASK			(0x0007)
#define DISPLAY_MIN_T_N_MASK			(0x0003)
#define DISPLAY_HOUR_O_N_MASK			(0x0007)
#define DISPLAY_HOUR_T_N_MASK			(0x0003)


#define DISPLAY_BUFF_LEN (DISPLAY_ROW_N * 2)

#define TIME_SECONDS_PER_MINUTE     60u
#define TIME_MINUTES_PER_HOUR       60u
#define TIME_HOURS_PER_DAY          24u
#define TIME_SECONDS_PER_HOUR       (60u*60u)
#define TIME_SECONDS_PER_DAY        (24ul*60ul*60ul)

#define TIME_SECONDS_MAX    (TIME_SECONDS_PER_MINUTE-1u)
#define TIME_MINUTES_MAX    (TIME_MINUTES_PER_HOUR-1u)
#define TIME_HOURS_MAX      (TIME_HOURS_PER_DAY-1u)

enum timeEnum
{
	TIME_MIN_O = 0,
	TIME_MIN_T,
	TIME_HOUR_O,
	TIME_HOUR_T,
	TIME_EN_N
};

volatile uint8_t displayBuff[DISPLAY_BUFF_LEN];

uint16_t randGenDots(uint8_t nDots, uint8_t nMax);
void genDotBuffers(uint8_t BCDhours, uint8_t BCDmins, uint8_t *displayBuffPtr);
void dotBuffUpdate(const uint8_t *displayBuffPtr, uint8_t enable);


#endif /* DOT_DISPLAY_H_ */
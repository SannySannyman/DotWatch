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
#include <string.h>

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

#define DISPLAY_MIN_MASK			(DISPLAY_MIN_O_MASK|DISPLAY_MIN_T_MASK)

#define DISPLAY_ALL_MASK			(0x03FF)
#define DISPLAY_OFF_MASK			(0x0000)

#define DISPLAY_OFF_MIN_O_MASK			(DISPLAY_ALL_MASK & ~(DISPLAY_MIN_O_MASK))
#define DISPLAY_OFF_MIN_T_MASK			(DISPLAY_ALL_MASK & ~(DISPLAY_MIN_T_MASK))
#define DISPLAY_OFF_HOUR_O_MASK			(DISPLAY_ALL_MASK & ~(DISPLAY_HOUR_O_MASK))
#define DISPLAY_OFF_HOUR_T_MASK			(DISPLAY_ALL_MASK & ~(DISPLAY_HOUR_T_MASK))

#define DISPLAY_MIN_O_ROW_WIDTH			3
#define DISPLAY_MIN_T_ROW_WIDTH			2
#define DISPLAY_HOUR_O_ROW_WIDTH		3
#define DISPLAY_HOUR_T_ROW_WIDTH		2

#define DISPLAY_MIN_O_N_MASK			(0x0007)
#define DISPLAY_MIN_T_N_MASK			(0x0003)
#define DISPLAY_HOUR_O_N_MASK			(0x0007)
#define DISPLAY_HOUR_T_N_MASK			(0x0003)

#define DISPLAY_BUFF_LEN (DISPLAY_ROW_N * 2)


enum timeEnum
{
	TIME_MIN_O = 0,
	TIME_MIN_T,
	TIME_HOUR_O,
	TIME_HOUR_T,
	TIME_EN_N
};


void displayInit(void);
uint16_t randGenDots(uint8_t nDots, uint8_t nMax);
void genDotBuffers(uint8_t BCDhours, uint8_t BCDmins);
void dotBuffUpdate(uint16_t enableMask);


#endif /* DOT_DISPLAY_H_ */
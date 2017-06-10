/*
 * main.c
 *
 * Created: 26.09.2016 17:50:28
 * Author : Oleksandr
 */ 

 #define UART_BAUD_RATE 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>
#include "common.h"
#include "bits.h"
#include "uart.h"
#include "i2cmaster.h"
#include "DS1307.h"
#include "menu.h"

#define TEMP_BUFF_SIZE 64
#define MAX_N_CHAR	10
char tempBuff[TEMP_BUFF_SIZE];

volatile uint8_t halfSecFlag = TRUE;


volatile uint8_t btnStatus = 0;
volatile uint16_t btnHoldCnt = 0;


#define  uartPrint(...) do {snprintf(tempBuff, TEMP_BUFF_SIZE, __VA_ARGS__); uart_puts(tempBuff);} while (0);

ISR(INT0_vect)
{
	halfSecFlag = TRUE;
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t btnTempStatus = 0;
	static uint16_t btnTempHoldCnt = 0;

	if (!check_bit(MODE_BTN_PIN, MODE_BTN_PIN_N))
	{
		set_bit(btnTempStatus, STATUS_MODE_BTN);
	}
	
	if (!check_bit(INC_BTN_PIN, INC_BTN_PIN_N))
	{
		set_bit(btnTempStatus, STATUS_INC_BTN);
	}


	if ((!check_bit(MODE_BTN_PIN, MODE_BTN_PIN_N))||
		(!check_bit(INC_BTN_PIN, INC_BTN_PIN_N)))
	{
		btnTempHoldCnt ++;
	}
	else if (btnTempHoldCnt > 1)
	{
		btnHoldCnt = btnTempHoldCnt;
		btnStatus = btnTempStatus;
		btnTempStatus = 0;
		btnTempHoldCnt = 0;
	}
	
}


void timeParse(char inChar)
{
	uint8_t temp = 0;
	static timeGetState_type state = TIME_GET_START_SYM;
	static time_type tempTime = {0, 0, 0};

	if ((state!=TIME_GET_SEPARATOR1)&&
	(state!=TIME_GET_SEPARATOR2)&&
	(state!=TIME_GET_START_SYM))
	{
		if (inChar > '9'|| inChar < '0')
		{
			inChar = '0';
			state = TIME_GET_IDLE;
		}
		
		temp = inChar - '0';
	}

	switch(state)
	{
		case TIME_GET_START_SYM:
		if (inChar != 'T')
		{
			state = TIME_GET_IDLE;
		}
		break;

		case TIME_GET_HOURS10:
		tempTime.Hours = temp*10;
		break;

		case TIME_GET_HOURS1:
		tempTime.Hours += temp;
		break;

		case TIME_GET_SEPARATOR1:
		case TIME_GET_SEPARATOR2:
		if (inChar != ':')
		{
			state = TIME_GET_IDLE;
		}
		break;

		case TIME_GET_MINUTES10:
		tempTime.Minutes = temp*10;
		break;

		case TIME_GET_MINUTES1:
		tempTime.Minutes += temp;
		break;

		case TIME_GET_SECONDS10:
		tempTime.Seconds = temp*10;
		break;

		case TIME_GET_SECONDS1:
		tempTime.Seconds += temp;

		if (tempTime.Hours > RTC_HOURS_MAX)
		{
			tempTime.Hours = 0;
		}

		if (tempTime.Minutes > RTC_MINUTES_MAX)
		{
			tempTime.Minutes = 0;
		}

		if (tempTime.Seconds > RTC_SECONDS_MAX)
		{
			tempTime.Seconds = 0;
		}

		tempTime.Hours = RTC_UintToBCD(tempTime.Hours);
		tempTime.Minutes = RTC_UintToBCD(tempTime.Minutes);
		tempTime.Seconds = RTC_UintToBCD(tempTime.Seconds);

		RTC_TimeSet(&tempTime);
		uart_puts("time updated\r\n");

		break;

		default:
		break;
	}

	if (state == TIME_GET_IDLE)
	{
		uart_puts("wrong time format\r\n");
	}


	//uartPrint("parsing, temp = %.3u, state = %.2u\r\n", temp, state);

	state++;
	if (state >= TIME_GET_STATE_NUM)
	{
		state = TIME_GET_START_SYM;
	}
}



int main(void)
{	
	time_type time = {0, 0, 0};
	uint16_t temp = 0;

	//uint8_t cnt = 0;

	set_bit(PORTD, PIND2);		//pull-up
	//set_bit2(MCUCR, ISC01, ISC00);	//rising edge on Int0 generate interrupt
	set_bit(MCUCR, ISC00);	//any change on Int0 generate interrupt
	set_bit(GICR, INT0);		//enable int0

	set_bit2(TCCR0, CS02, CS00);	//timer 0, ~30Hz
	set_bit(TIMSK, TOIE0);			//enable overflow interrupt

	displayInit();

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	i2c_init();
	sei();

	RTC_Init();

	MenuInit();

    while (1) 
    {
		if (halfSecFlag == TRUE)
		{
			halfSecFlag = FALSE;

			

			if (check_bit(PIND, PIND2))//positive edge
			{
				DisplayMaskToggle();
				DisplayUpdate(UPD_US);

				RTC_TimeGet(&time);
				uartPrint("%.2x:%.2x:%.2x",time.Hours, time.Minutes, time.Seconds);
				uart_puts("\n\r\n\r");
			}

			
		}

		if (btnStatus != 0)
		{
			uartPrint("BTNStatus = %.2x, HoldTime = %u\r\n", btnStatus, btnHoldCnt);
			DisplayMenuUpdate(btnStatus, btnHoldCnt);
			btnStatus = 0;
		}

		temp = uart_getc();
		if ((temp & 0xFF00) == 0)// no errors and new data
		{
			timeParse((char)temp);
		}

    }
}
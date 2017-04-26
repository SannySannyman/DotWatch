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
#include "dot_display.h"
#include "DS1307.h"
#include "menu.h"

#define TEMP_BUFF_SIZE 64
#define MAX_N_CHAR	10
char tempBuff[TEMP_BUFF_SIZE];

volatile uint8_t halfSecFlag = TRUE;


volatile uint8_t btnStatus = 0;
#define STATUS_MODE_BTN		0
#define STATUS_INC_BTN		1

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

ISR(SPI_STC_vect)
{
	static uint8_t cnt = 0;

	SPDR = displayBuff[cnt];

	if (cnt%2 == 0)
	{
		set_bit(D_UPD_PORT, D_UPD_PIN);
		unset_bit(D_UPD_PORT, D_UPD_PIN);
	}

	cnt++;
	if (cnt >= DISPLAY_BUFF_LEN)
	{
		cnt = 0;
	}
}

//void uartPrintUNum(uint16_t number,  uint8_t nChar, uint8_t radix)
//{
	//char buff[MAX_N_CHAR];
	//uint8_t nCharNum = 0;
//
	//if (nChar >= MAX_N_CHAR)
	//{
		//uart_puts("ERRN");
		//return;
	//}
//
	//nCharNum = (number/radix)+1;
//
	//memset(buff,'0',MAX_N_CHAR);
	//itoa(number, buff, radix);
	//uart_puts(buff);
//}

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

		if (tempTime.Hours > TIME_HOURS_MAX)
		{
			tempTime.Hours = 0;
		}

		if (tempTime.Minutes > TIME_MINUTES_MAX)
		{
			tempTime.Minutes = 0;
		}

		if (tempTime.Seconds > TIME_SECONDS_MAX)
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
	time_type currentTime = {0, 0, 0};
	uint16_t temp = 0;
	uint8_t displayBuffTemp[DISPLAY_BUFF_LEN];
	
	set_bit(D_UPD_DDR, D_UPD_PIN);
	set_bit(D_CLK_DDR, D_CLK_PIN);
	set_bit(D_DATA_DDR, D_DATA_PIN);

	set_bit5(SPCR, SPIE, SPE, MSTR, SPR1, SPR0);	//SPI interrupt, SPI en, master, clk/128

	set_bit(PORTD, PIND2);		//pull-up
	//set_bit2(MCUCR, ISC01, ISC00);	//rising edge on Int0 generate interrupt
	set_bit(MCUCR, ISC00);	//any change on Int0 generate interrupt
	set_bit(GICR, INT0);		//enable int0

	set_bit2(TCCR0, CS02, CS00);	//timer 0, ~30Hz
	set_bit(TIMSK, TOIE0);			//enable overflow interrupt


	SPDR = 0x00;

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	i2c_init();
	sei();

	RTC_Init();

	memset(displayBuffTemp, 0, DISPLAY_BUFF_LEN);

    while (1) 
    {
		if (halfSecFlag == TRUE)
		{
			halfSecFlag = FALSE;

			if(isEditMode())
			{
				dotBuffUpdate(displayBuffTemp, check_bit(PIND, PIND2));
			}
			else if (check_bit(PIND, PIND2))
			{
				RTC_TimeGet(&currentTime);

				if (RTC_BCDToUint(currentTime.Seconds)%5 == 0)
				{
					genDotBuffers(currentTime.Hours, currentTime.Minutes, displayBuffTemp);
					dotBuffUpdate(displayBuffTemp, TRUE);
				}

				uartPrint("%.2x:%.2x:%.2x",currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
				uart_puts("\n\r\n\r");
			}
		}

		if (btnStatus != 0)
		{
			if (isEditMode())
			{
				genDotBuffers(0, btnHoldCnt, displayBuffTemp);
				dotBuffUpdate(displayBuffTemp, check_bit(PIND, PIND2));
			}

			uartPrint("BTNStatus = %.2x, HoldTime = %u\r\n", btnStatus, btnHoldCnt);
			btnStatus = 0;
		}

		temp = uart_getc();
		if ((temp & 0xFF00) == 0)// no errors and new data
		{
			timeParse((char)temp);
		}

    }
}
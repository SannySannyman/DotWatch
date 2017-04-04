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
#include <util/atomic.h>
#include "common.h"
#include "bits.h"
#include "uart.h"
#include "i2cmaster.h"
#include "dot_display.h"
#include "DS1307.h"

#define TEMP_BUFF_SIZE 64
volatile uint8_t displayUpdFlag = TRUE;
volatile uint8_t oneSecFlag = TRUE;


ISR(INT0_vect)
{
	oneSecFlag = TRUE;
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

int main(void)
{	
	char tempBuff[TEMP_BUFF_SIZE];
	time_type currentTime = {0, 0, 0};
	uint16_t temp = 0;
	
	set_bit(D_UPD_DDR, D_UPD_PIN);
	set_bit(D_CLK_DDR, D_CLK_PIN);
	set_bit(D_DATA_DDR, D_DATA_PIN);

	set_bit5(SPCR, SPIE, SPE, MSTR, SPR1, SPR0);	//SPI interrupt, SPI en, master, clk/128

	set_bit(PORTD, PIND2);		//pull-up
	set_bit2(MCUCR, ISC01, ISC00);	//rising edge on Int0 generate interrupt
	set_bit(GICR, INT0);		//enable int0


	SPDR = 0x00;

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	i2c_init();
	sei();

	RTC_Init();

    while (1) 
    {
		if (oneSecFlag == TRUE)
		{
			oneSecFlag = FALSE;

			RTC_TimeGet(&currentTime);

			if (RTC_BCDToUint(currentTime.Seconds)%5 == 0)
			{
				displayUpdFlag = TRUE;
			}

			snprintf(tempBuff, TEMP_BUFF_SIZE,"%.2x:%.2x:%.2x",currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
			uart_puts(tempBuff);
			uart_puts("\n\r\n\r");
		}

		if (displayUpdFlag == TRUE)
		{
			displayUpdFlag = FALSE;

			genDotBuffers(currentTime.Hours, currentTime.Minutes);
		}

		temp = uart_getc();
		if ((temp & 0xFF00) == 0)// no errors and new data
		{
			static timeGetState_type state = TIME_GET_START_SYM;
			static time_type tempTime = {0, 0, 0};

			if ((state!=TIME_GET_SEPARATOR1)&&
				(state!=TIME_GET_SEPARATOR2)&&
				(state!=TIME_GET_START_SYM))
			{
				if (temp > '9'|| temp < '0')
				{
					temp = '0';
					state = TIME_GET_IDLE;
				}
				
				temp = temp - '0';
			}

			switch(state)
			{
				case TIME_GET_START_SYM:
					if (temp != 'T')
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
					if (temp != ':')
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
				snprintf(tempBuff, TEMP_BUFF_SIZE,"wrong time format\r\n");
				uart_puts(tempBuff);
			}


			//snprintf(tempBuff, TEMP_BUFF_SIZE,"parsing, temp = %.3u, state = %.2u\r\n", temp, state);
			//uart_puts(tempBuff);

			state++;
			if (state >= TIME_GET_STATE_NUM)
			{
				state = TIME_GET_START_SYM;
			}
		}

    }
}
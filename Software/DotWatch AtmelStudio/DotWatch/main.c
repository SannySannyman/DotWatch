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

#define TEMP_BUFF_SIZE 16
volatile uint8_t displayUpdFlag = TRUE;
volatile uint8_t oneSecFlag = FALSE;


ISR(INT0_vect)
{
	static uint8_t cnt = 0;

	oneSecFlag = TRUE;

	cnt++;
	if (cnt >= 5)
	{
		cnt = 0;
		displayUpdFlag = TRUE;
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

int main(void)
{	
	char tempBuff[TEMP_BUFF_SIZE];
	time_type currentTime = {0, 0, 0};
	
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
			snprintf(tempBuff, TEMP_BUFF_SIZE, "%.2x:%.2x:%.2x ",currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
			uart_puts(tempBuff);
			uart_puts("\n\r\n\r");
		}

		if (displayUpdFlag == TRUE)
		{
			displayUpdFlag = FALSE;

			genDotBuffers(currentTime.Hours, currentTime.Minutes);
		}
    }
}
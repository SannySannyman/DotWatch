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
	#define TEMP_BUFF_SIZE 16
	char tempBuff[TEMP_BUFF_SIZE];
	
	set_bit(D_UPD_DDR, D_UPD_PIN);
	set_bit(D_CLK_DDR, D_CLK_PIN);
	set_bit(D_DATA_DDR, D_DATA_PIN);

	
	//set_bit2(TCCR0, CS01, CS00);	//clk/64
	//set_bit(TIMSK, TOIE0);	//ovf interrupt enable

	//OCR2 = 0x0F;
	//set_bit2(TCCR2, WGM21, CS22); //CTC, clk/64
	//set_bit(TIMSK, OCIE2);			//compare match interrupt

	set_bit5(SPCR, SPIE, SPE, MSTR, SPR1, SPR0);	//SPI interrupt, SPI en, master, clk/128

	SPDR = 0x00;

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	i2c_init();

	sei();

    while (1) 
    {
		
		genDotBuffers(0x21, 0x37);

		for(uint8_t cnt = 0; cnt< DISPLAY_BUFF_LEN; cnt++)
		{
			snprintf(tempBuff, TEMP_BUFF_SIZE, "%.2x ", displayBuff[cnt]);
			uart_puts(tempBuff);
		}

		uart_puts("\n\r\n\r");
		
		_delay_ms(5000);
	
    }
}
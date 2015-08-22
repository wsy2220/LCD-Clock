#include "avr.h"

#define SCK PG0
#define IO  PG1
#define CE  PG2
#define PORT_RTC PORTG
#define DDR_RTC  DDRG

#define W_ENABLE DDR_RTC |=  _BV(IO)
#define R_ENABLE DDR_RTC &= ~_BV(IO)
#define CE_HIGH  PORT_RTC |=  _BV(CE)
#define CE_LOW   PORT_RTC &= ~_BV(CE)
#define SCK_HIGH PORT_RTC |= _BV(SCK)
#define SCK_LOW  PORT_RTC &= ~_BV(SCK)
#define IO_HIGH PORT_RTC |= _BV(IO)
#define IO_LOW  PORT_RTC &= ~_BV(IO)

#define T_DC  0.25
#define T_CH  0.5
#define T_CL  0.5
#define T_CC  4
#define T_CWH 4


void rtc_init()
{
	DDR_RTC |= _BV(SCK) | _BV(CE);
	CE_LOW;
	_delay_us(100);
}

/* Must be followed immediately by read or write */
static void rtc_send(char b)
{
	W_ENABLE;
	CE_HIGH;
	_delay_us(T_CC);
	int i;
	for(i = 0; i < 8; i++){
		if(((b >> i) & 0x01))
			IO_HIGH;
		else
			IO_LOW;
		_delay_us(T_DC);
		SCK_HIGH;
		_delay_us(T_CH);
		SCK_LOW;
		_delay_us(T_CL);
	}
}

static char rtc_read()
{
	R_ENABLE;
	char b = 0;
	int i;
	for(i = 0; i < 8; i++){
		//b |= ((PORT_RTC >> IO) & 0x01) << i;
		if((PORT_RTC & _BV(IO)))
			b |= 1;
		b <<= 1;
		SCK_HIGH;
		_delay_us(T_CH);
		SCK_LOW;
		_delay_us(T_CL);
	}
	CE_LOW;
	_delay_us(T_CWH);
	return b ;
}

static void rtc_write(char b)
{
	int i;
	for(i = 0; i < 8; i++){
		PORT_RTC |= ((b >> i) & 0x01) * _BV(IO);
		_delay_us(T_DC);
		SCK_HIGH;
		_delay_us(T_CH);
		SCK_LOW;
		_delay_us(T_CL);
	}
	CE_LOW;
	_delay_us(T_CWH);
}

char rtc_read_sec()
{
	char sec;
	rtc_send(0x81);
	sec = rtc_read();
	return sec;
}


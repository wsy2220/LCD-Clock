#include "avr.h"

#define SCK PG0
#define IO  PG1
#define CE  PG2
#define PORT_RTC PORTG
#define DDR_RTC  DDRG
#define PIN_RTC  PING

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
#define T_CC  2
#define T_CWH 2


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
		b |= ((PIN_RTC >> IO) & 0x01) << i;
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
	CE_LOW;
	_delay_us(T_CWH);
}

/* read a register using raw command, see datasheet */
char rtc_read_reg(char cmd)
{
	rtc_send(cmd);
	return rtc_read();
}

void rtc_write_reg(char cmd, char b)
{
	rtc_send(cmd);
	rtc_write(b);
}

#include "avr.h"

#define LCD_RS PD5
#define LCD_RW PD7
#define LCD_E  PD6
#define LCD_VCC PD4

#define DDR_CTL  DDRD
#define PORT_CTL PORTD
#define DDR_DATA DDRC
#define PORT_DATA PORTC /*B0-B7 connected to PC0-PC7 */
#define PIN_DATA PINC

#define W_ENABLE DDR_DATA = 0xFF
#define R_ENABLE DDR_DATA = 0

#define E_HIGH PORT_CTL |=  _BV(LCD_E)
#define E_LOW  PORT_CTL &= ~_BV(LCD_E)

/* T_EH cover T_DDR */
#define T_EH 0.23 
#define T_EL 0.27 


/* read address & busyflag byte */
static char read_adb()
{
	char adb;
	R_ENABLE;
	PORT_CTL &= ~_BV(LCD_RS);
	PORT_CTL |=  _BV(LCD_RW);
	E_HIGH;
	_delay_us(T_EH);
	adb = PIN_DATA;
	E_LOW;
	_delay_us(T_EL);
	return adb;
}

static char isBusy()
{
	return read_adb() & 0x80;
}

static void write_IR(char I)
{
	while(isBusy())
		;
	W_ENABLE;
	PORT_CTL &= ~(_BV(LCD_RS) | _BV(LCD_RW));
	PORT_DATA = I;
	E_HIGH;
	_delay_us(T_EH);
	E_LOW;
	_delay_us(T_EL);
}

static void write_DR(char D)
{
	while(isBusy())
		;
	W_ENABLE;
	PORT_CTL &= ~_BV(LCD_RW);
	PORT_CTL |=  _BV(LCD_RS);
	PORT_DATA = D;
	E_HIGH;
	_delay_us(T_EH);
	E_LOW;
	_delay_us(T_EL);
}

void lcd_init()
{
	DDR_CTL |= _BV(LCD_RS) | _BV(LCD_RW) | _BV(LCD_E) | _BV(LCD_VCC);
	E_LOW;
	PORT_CTL |= _BV(LCD_VCC);
	_delay_ms(20);
	write_IR(0x38);
	write_IR(0x0C);
}

void print_char(char c)
{
	write_DR(c);
}

void set_cursor(char isOn, char isBlinking)
{
	char cmd = 0x0C;
	if(isOn)
		cmd |= 0x02;
	else
		cmd &= ~0x02;
	if(isBlinking)
		cmd |= 0x01;
	else
		cmd &= ~0x01;
	write_IR(cmd);
}

/* line: 0-1, column: 0-40 */
void set_position(char line, char column)
{
	char addr = (line << 6) + column + 0x80;
	write_IR(addr);
}

void clear_screen()
{
	write_IR(0x01);
}

void return_home()
{
	write_IR(0x02);
}

void print_string(char *buf, char size)
{
	int i;
	for(i = 0; i < size; i++)
		print_char(buf[i]);
}

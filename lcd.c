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


static void write_IR(char I)
{
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
	_delay_us(40);
	write_IR(0x0E);
	_delay_us(40);
}

void print_char(char c)
{
	write_DR(c);
	_delay_us(40);
}

/* line: 0-1, column: 0-40 */
void set_position(char line, char column)
{
	char addr = (line << 6) + column + 0x80;
	write_IR(addr);
	_delay_us(40);
}

void clear_screen()
{
	write_IR(0x01);
	_delay_ms(2);
}

void return_home()
{
	write_IR(0x02);
	_delay_ms(2);
}

void print_string(char *buf, char size)
{
	int i;
	for(i = 0; i < size; i++)
		print_char(buf[i]);
}

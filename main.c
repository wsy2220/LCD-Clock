#include "avr.h"
#include "lcd.h"
#include "rtc.h"

void bcd2char(char b, char *buf)
{
	buf[0] = ((b >> 4) & 0x0F) + 0x30;
	buf[1] = (b & 0x0F) + 0x30;
}


int main()
{
	lcd_init();
	rtc_init();
	DDRB = 0xF0;
	char sec, sec_str[2];
	for(;;){
		sec = rtc_read_sec();
		bcd2char(sec, sec_str);
		set_position(0,0);
		print_string(sec_str, 2);
		PORTB = sec & 0xF0;
		_delay_ms(100);
	}
	return 0;

}

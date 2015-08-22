#include "avr.h"
#include "lcd.h"
#include "rtc.h"

/* stating colum of time.date and time */
#define DATE_START 3
#define TIME_START 4
#define DATE_LINE  0
#define TIME_LINE  1


enum{DISP, SET};
char mode, line = DATE_LINE, column = DATE_START + 2, isChanged ;
char days[12] = {0x31,0x28,0x31,0x30,0x31,0x30,0x31,0x31,0x30,0x31,0x30,0x31};
/* same sequence and format of ds1302 register */
struct{
	char second;
	char minute;
	char hour;
	char date;
	char month;
	char dayofweek;
	char year;
	char WP;
	char TCS;
} time;


void bcd2char(char b, char *buf)
{
	buf[0] = ((b >> 4) & 0x0F) + 0x30;
	buf[1] = (b & 0x0F) + 0x30;
}

char bcd2byte(char b)
{
	return (b & 0xF) + ((b >> 4) & 0xF) * 10;
}

/* force unsigned compare */
char min(char a, char b)
{
	if((unsigned char)a > (unsigned char)b)
		return b;
	else
		return a;
}
	

/* add 0x01 or 0x10 to bcd code a */
char bcd_add1(char a, char b)
{
	 a += b;
	if(b == 0x01 && (a & 0xF) > 0x9)
		a &= 0xF0;
	if(b == 0x10 && (a & 0xF0) > 0x90)
		a &= 0x0F;
	return a;
}

void print_time()
{
	char buf[2];
	set_position(DATE_LINE, DATE_START);
	print_string("20", 2);
	bcd2char(time.year, buf);
	print_string(buf, 2);
	print_char('.');
	bcd2char(time.month, buf);
	print_string(buf, 2);
	print_char('.');
	bcd2char(time.date, buf);
	print_string(buf, 2);

	set_position(TIME_LINE, TIME_START);
	bcd2char(time.hour, buf);
	print_string(buf, 2);
	print_char(':');
	bcd2char(time.minute, buf);
	print_string(buf, 2);
	print_char(':');
	bcd2char(time.second, buf);
	print_string(buf, 2);
}

void get_rtc()
{
	time.second = rtc_read_reg(SEC_R);
	time.minute = rtc_read_reg(MIN_R);
	time.hour   = rtc_read_reg(HR_R);
	time.date   = rtc_read_reg(DAT_R);
	time.month  = rtc_read_reg(MON_R);
	time.year   = rtc_read_reg(YR_R);
}

void button_init()
{
	DDRD &= 0xF0;
	PORTD |= 0x0F;
	/* enable interrupts on falling edge */
	EICRA = 0xAA;
	EIMSK |= 0x0F;
}

void check_time()
{
	time.second = min(time.second, 0x60);
	time.minute = min(time.minute, 0x60);
	time.hour   = min(time.hour,   0x24);
	/*check month first to make it valid */
	time.month  = min(time.month,  0x12);
	if(time.month == 2){
		if(time.year % 4) /* 2000-2099 only */
			time.date = min(time.date, 0x28); 
		else
			time.date = min(time.date, 0x29); 
	}
	else
		time.date = min(time.date, days[bcd2byte(time.month)-1]);
}

void set_time()
{
	rtc_write_reg(SEC_W, time.second);
	rtc_write_reg(MIN_W, time.minute);
	rtc_write_reg(HR_W , time.hour  );
	rtc_write_reg(DAT_W, time.date  );
	rtc_write_reg(MON_W, time.month );
	rtc_write_reg(YR_W , time.year  );
}


ISR(INT0_vect)
{
	if(mode == DISP){
		mode = SET;
		set_position(line, column);
		set_cursor(1, 0);
	}
	else{
		mode = DISP;
		if(isChanged){
			isChanged = 0;
			check_time();
			set_time();
		}
		set_position(DATE_LINE, DATE_START);
		set_cursor(0, 0);
	}
}

/* move cursor to the right */
ISR(INT1_vect)
{
	if(mode == DISP)
		reti();
	if(line == DATE_LINE){
		if(column == DATE_START + 3 ||
		   column == DATE_START + 6 )  /* skip separator */
			column += 2;
		else if(column == DATE_START + 9){
			column = TIME_START;
			line   = TIME_LINE;
		}
		else
			column++;
	}
	else{
		if(column == TIME_START + 1 ||
		   column == TIME_START + 4 )
			column += 2;
		else if(column == TIME_START + 7){
			column = DATE_START + 2;
			line   = DATE_LINE;
		}
		else
			column++;
	}
	set_position(line, column);
}

ISR(INT2_vect)
{
	if(mode == DISP)
		reti();
	if(line == DATE_LINE){
		if(column == DATE_START + 5 ||
		   column == DATE_START + 8 )  /* skip separator */
			column -= 2;
		else if(column == DATE_START + 2){
			column = TIME_START + 7;
			line   = TIME_LINE;
		}
		else
			column--;
	}
	else{
		if(column == TIME_START + 3 ||
		   column == TIME_START + 6 )
			column -= 2;
		else if(column == TIME_START ){
			column = DATE_START + 9;
			line   = DATE_LINE;
		}
		else
			column--;
	}
	set_position(line, column);
}

ISR(INT3_vect)
{
	if(mode == DISP)
		reti();
	isChanged = 1;
	if(line == DATE_LINE){
		switch(column){
			case DATE_START + 2:
				time.year = bcd_add1(time.year, 0x10);
				break;
			case DATE_START + 3:
				time.year = bcd_add1(time.year, 0x01);
				break;
			case DATE_START + 5:
				time.month = bcd_add1(time.month, 0x10);
				break;
			case DATE_START + 6:
				time.month = bcd_add1(time.month, 0x01);
				break;
			case DATE_START + 8:
				time.date = bcd_add1(time.date, 0x10);
				break;
			case DATE_START + 9:
				time.date = bcd_add1(time.date, 0x01);
				break;
		}
	}
	else{
		switch(column){
			case TIME_START + 0:
				time.hour = bcd_add1(time.hour, 0x10);
				break;
			case TIME_START + 1:
				time.hour = bcd_add1(time.hour, 0x01);
				break;
			case TIME_START + 3:
				time.minute = bcd_add1(time.minute, 0x10);
				break;
			case TIME_START + 4:
				time.minute = bcd_add1(time.minute, 0x01);
				break;
			case TIME_START + 6:
				time.second = bcd_add1(time.second, 0x10);
				break;
			case TIME_START + 7:
				time.second = bcd_add1(time.second, 0x01);
				break;
		}
	}
	print_time();
	set_position(line, column);
}

int main()
{
	lcd_init();
	rtc_init();
	PORTB = 0xF0;
	DDRB  = 0xF0;
	button_init();
	for(;;){
		if(mode == DISP){
			cli();
			get_rtc();
			print_time();
			sei();
		}
		_delay_ms(5);
	}
	return 0;
}

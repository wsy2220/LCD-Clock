#include <avr/io.h>
#define F_CPU 16E6
#include <util/delay.h>

int main()
{
	DDRB = 0xF0;
	PORTD = 0xFF;
	for(;;){
		PORTB=~(PIND << 4) ;
	}

	return 0;
}

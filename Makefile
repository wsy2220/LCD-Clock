CC = avr-gcc
CFLAGS = -Os -mmcu=atmega128a -Wall
prj = clock
obj = lcd.o rtc.o main.o

hex: elf
	objcopy -O ihex -j .text -j .data $(prj).elf $(prj).hex

elf: $(obj)
	$(CC) $(CFLAGS) $(obj) -o $(prj).elf

clean:
	rm $(obj) $(prj).elf $(prj).hex

flash: hex
	avrdude -p m128 -c jtag1 -P /dev/ttyUSB0 -U flash:w:$(prj).hex

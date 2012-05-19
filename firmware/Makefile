CC=avr-gcc
CFLAGS += -DF_CPU=16000000UL
CFLAGS += -DF_BAUDRATE=9600
CFLAGS += -mmcu=atmega644
CFLAGS += -Os
all: main.o
	avr-gcc -mmcu=atmega644 -o firmware $^
	avr-objcopy -O ihex -R .eeprom firmware firmware.hex
	avr-size --mcu=atmega644 -C firmware

clean:
	rm firmware.hex main.o

flash:
	avrdude -c usbasp -p atmega644 -U firmware.hex


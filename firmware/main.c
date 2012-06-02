/*
 * vim:ts=4:sw=4:expandtab
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#define BAUD 9600
#include <util/delay.h>
#include <stdio.h>
#include "uart.c"


// Count the number of overflows
volatile uint64_t overflow_counter = 0;
// Last overflow counter, necessary for temporarely storing that value
volatile uint64_t last_overflow_counter = 0;
// Number of received pulses
volatile uint64_t pulse_counter = 0;
// Last number of received pulses, necessary for temporarely storing that value
volatile uint64_t last_pulse_counter = 0;
// Last timer counter state
volatile uint16_t last_timer_state = 0;
// 
volatile bool pulse_detected = false;


ISR(TIMER1_OVF_vect) {
	overflow_counter++;
}

ISR(INT0_vect) {
	pulse_counter++;
	// Prevent the code from setting the temporary values if the main loop didn't finish processing the last pulse
	if (!pulse_detected) {
		last_timer_state = TCNT1;
		last_overflow_counter = overflow_counter;
		last_pulse_counter = pulse_counter;
		pulse_detected = true;
	}
}

const char hex_chars[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

#define uart_put_hex(character) \
uart_putc(hex_chars[((character)>>4)&0x0f]); \
uart_putc(hex_chars[((character)>>0)&0x0f]); 


#define uart_put_64(character) \
uart_putc(hex_chars[((character)>>60)&0x0f]); \
uart_putc(hex_chars[((character)>>56)&0x0f]); \
uart_putc(hex_chars[((character)>>52)&0x0f]); \
uart_putc(hex_chars[((character)>>48)&0x0f]); \
uart_putc(hex_chars[((character)>>44)&0x0f]); \
uart_putc(hex_chars[((character)>>40)&0x0f]); \
uart_putc(hex_chars[((character)>>36)&0x0f]); \
uart_putc(hex_chars[((character)>>32)&0x0f]); \
uart_putc(hex_chars[((character)>>28)&0x0f]); \
uart_putc(hex_chars[((character)>>24)&0x0f]); \
uart_putc(hex_chars[((character)>>20)&0x0f]); \
uart_putc(hex_chars[((character)>>16)&0x0f]); \
uart_putc(hex_chars[((character)>>12)&0x0f]); \
uart_putc(hex_chars[((character)>>8)&0x0f]); \
uart_putc(hex_chars[((character)>>4)&0x0f]); \
uart_putc(hex_chars[((character)>>0)&0x0f]); 

#define uart_put_16(character) \
uart_putc(hex_chars[((character)>>12)&0x0f]); \
uart_putc(hex_chars[((character)>>8)&0x0f]); \
uart_putc(hex_chars[((character)>>4)&0x0f]); \
uart_putc(hex_chars[((character)>>0)&0x0f]); 

int main() {

	// Use default timer mode (no compare match, no pwm)
	TCCR1A = 0;
	// Clock select 101 for CPU_FREQ/1024
	TCCR1B = (1<<CS10)|(1<<CS12);

	TIMSK1 = (1<<TOIE1);


	// Configure PD2 as input
	DDRD = 0;
	// Enable pullup for PD2
	PORTD = (1<<2);

	// Enable INT0 interrupts
	EIMSK = (1<<INT0);

	// Configure INT0 interrupt to trigger for falling edge
	EICRA = (1<<ISC01);
   
	uart_init( UART_BAUD_SELECT(F_BAUDRATE,F_CPU) );
	// Enable interrupts
	sei();
	
	uart_puts("\r\n");

	for (;;) {
		if (pulse_detected) {
			uart_put_64(last_pulse_counter);
			uart_putc('\t');
			uart_put_64(last_overflow_counter);
			uart_putc('\t');
			uart_put_16(last_timer_state);
			uart_puts("\r\n");
			pulse_detected = false;
		}
	}

	return 0;
}


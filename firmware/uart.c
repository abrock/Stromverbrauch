/*** uart.c ***/
#include <avr/io.h>
#include "uart.h"

void init_uart(void)
{
  UBRR0H = (unsigned char)(UBRR_VAL>>8);
  UBRR0L = (unsigned char)(UBRR_VAL);
  UCSR0B = (1<<TXEN0) | (1<<RXEN0);      // tx & rx enable
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);    // Asynchron 8N1
}

void uart_putc(unsigned char c)
{
  while (!(UCSR0A & (1<<UDRE0)))
  {
    /* warten bis Senden moeglich */
  }
  UDR0 = c;
}

uart.h

/*** uart.h ***/
#ifndef uart_h
#define uart_h

#define BAUD 9600UL          // Baudrate

#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun nachgeholt mit 14745600UL"
#define F_CPU 14745600UL    // Systemtakt in Hz
// Definition als unsigned long beachten
// Ohne UL ergibt Fehler in der Berechnung
#endif
 
// Berechnungen
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)      
// Fehler in Promille, 1000 = kein Fehler
 
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch! 
#endif

void init_uart(void);
void uart_putc(unsigned char c);
#endif /* uart_h */

main.c

/*** main.c ***/
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

int main(void)
{
  init_uart();
  _delay_ms(10);

  uart_putc('#'); // vor while

  while(1)
  {
    uart_putc('=');  // in while
  }

  return 0;
}



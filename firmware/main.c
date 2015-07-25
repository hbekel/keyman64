#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"

#define MC 1<<PA0
#define MD 1<<PA1

ISR(PCINT3_vect) { RelayMatrix(); }

static volatile uint8_t matrix[8] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static void SetupHardware(void) {

  clock_prescale_set(clock_div_1);

  DisableJTAG();
  
  // Control / Output 8-11
  DDRA  = 0b11110001;
  PORTA = 0b00001110;

  // CIA1 PORT A
  DDRD  = 0b00000000;
  PORTD = 0b00000000;

  // Output 0-7
  DDRC  = 0b11111111;
  PORTC = 0b00000000;

  // CIA1 PORT B
  DDRB  = 0b11111111;
  PORTB = 0b00000000;

  Settle();

  PCMSK3 = 0xff;
  PCICR |= (1<<PCIE3);
  sei();
}

static void DisableJTAG(void) {
  MCUCR |= (1<<JTD);
  MCUCR |= (1<<JTD);
}

static void Settle(void) {
  _delay_ms(10);
}  

static void ClockMatrix(void) {
  PORTA |= MC;
  _delay_us(5);
  PORTA &= ~MC;
}

static void ScanMatrix(void) {
  uint8_t row = 0;
  uint8_t col = 0;
  
  for(row=0; row<8; row++) {
    for(col=0; col<8; col++) {
      if((PINA & MD) == 0) {
        matrix[row] &= ~(1<<col);
      }
      else {
        matrix[row] |= 1<<col;
      }
      ClockMatrix();
    }
  }
  PORTC = matrix[0];
}

static void RelayMatrix(void) {
  uint8_t row = 0;
  uint8_t col = 0xff;
  uint8_t tmp = PIND;
  
  for(row=0; row<8; row++) {
    if((tmp & (1<<row)) == 0) {
      col &= matrix[row];
    }
  }
  PORTB = col;
}

int main(void) {
  SetupHardware();
  
  while(1) {
    ScanMatrix();
  }

  return 0;
}

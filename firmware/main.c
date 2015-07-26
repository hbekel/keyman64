#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"

#define MC 1<<PD3
#define MD 1<<PD4
#define MR 1<<PD0

static volatile uint8_t matrix[8] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static void SetupHardware(void) {

  clock_prescale_set(clock_div_1);

  DisableJTAG();
  
  // Crosspoint Control
  DDRA  = 0b11111111;
  PORTA = 0b01000000;

  // User port A
  DDRB  = 0b11111111;
  PORTB = 0b00000000;

  // User port B
  DDRC  = 0b11111111;
  PORTC = 0b00000000;

  // USB, Bootloader and Matrix Control
  DDRD  = 0b10001111;
  PORTD = 0b11110110;
}

static void DisableJTAG(void) {
  MCUCR |= (1<<JTD);
  MCUCR |= (1<<JTD);
}

static void ResetMatrix(void) {
  PORTD |= MR;
  PORTD &= ~MR;
}

static void ClockMatrix(void) {
  PORTD |= MC;
  _delay_us(5);
  PORTD &= ~MC;
}

static void ScanMatrix(void) {
  uint8_t row = 0;
  uint8_t col = 0;

  ResetMatrix();
  
  for(row=0; row<8; row++) {
    for(col=0; col<8; col++) {
      if((PIND & MD) == 0) {
        matrix[row] &= ~(1<<col);
      }
      else {
        matrix[row] |= 1<<col;
      }
      ClockMatrix();
    }
  }
  PORTB = matrix[0];
}

int main(void) {
  SetupHardware();
  
  while(1) ScanMatrix();

  return 0;
}

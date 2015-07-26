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

#define RELAY   0x00
#define COMMAND 0x01

uint8_t STATE = RELAY;

static volatile uint8_t matrix[8] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

static void DisableJTAG(void) {
  MCUCR |= (1<<JTD);
  MCUCR |= (1<<JTD);
}

//------------------------------------------------------------------------------

static void ResetCounter(void) {
  PORTD |= MR;
  PORTD &= ~MR;
}

//------------------------------------------------------------------------------

static void ClockMatrix(void) {
  PORTD |= MC;
  _delay_us(5);
  PORTD &= ~MC;
}

//------------------------------------------------------------------------------

static bool ScanMatrix(void) {
  bool keyDown = false;
  uint8_t row = 0;
  uint8_t col = 0;  
  
  ResetCounter();
  
  for(row=0; row<8; row++) {
    for(col=0; col<8; col++) {
      if((PIND & MD) == 0) {
        matrix[row] &= ~(1<<col);
        keyDown = true;
      }
      else {
        matrix[row] |= 1<<col;
      }
      ClockMatrix();
    }
  }
  return keyDown;
}

//------------------------------------------------------------------------------

static void RelayMatrix(void) {
  // IMPLEMENTME
}

//------------------------------------------------------------------------------

static void RelayKeyPress(key_t key) {
  // IMPLEMENTME
}

//------------------------------------------------------------------------------

static bool KeysEqual(key_t key, key_t other) {
  return key.col == other.col && key.row == other.row;
}

//------------------------------------------------------------------------------

static bool QueryKeyPress(key_t key) {

  /* If the requested key is down, wait until it is released. */
  
  bool result = false;
  
  if(ScanMatrix() && (matrix[key.row] & 1<<key.col) == 0) {
    _delay_ms(20);
    if(ScanMatrix() && (matrix[key.row] & 1<<key.col) == 0) {
      result = true;
      while(ScanMatrix() && (matrix[key.row] & 1<<key.col) == 0);
    }
  }
  return result;
}

//------------------------------------------------------------------------------

static void ReadKeyPress(key_t* key) {

  /* Wait until a key is down, then wait for release and report back
   * which key was pressed.
  */

  uint8_t row = 0;
  uint8_t col = 0;  
  
  while(!ScanMatrix());

  _delay_ms(20);
  
  if(ScanMatrix()) {
    for(row=0; row<8; row++) {
      for(col=0; col<8; col++) {
        if((matrix[row] & 1<<col) == 0) {
          key->row = row;
          key->col = col;
          goto waitForKeyRelease;
        }
      }
    }

  waitForKeyRelease:
    while(ScanMatrix() && (matrix[key->row] & 1<<key->col) == 0);
  }
}

//------------------------------------------------------------------------------

int main(void) {
  SetupHardware();

  key_t key = { .col = 0, .row = 0 };
  
  while(true) {
    
    switch(STATE) {

    case RELAY:
      PORTB &= ~1;
      if(QueryKeyPress(KEY_ARROW_LEFT)) {
        STATE = COMMAND;
      }
      else {
        RelayMatrix();
      }
      break;
      
    case COMMAND:
      PORTB |= 1;

      ReadKeyPress(&key);
      
      if(KeysEqual(key, KEY_ARROW_LEFT)) {
        RelayKeyPress(KEY_ARROW_LEFT);
        STATE = RELAY;
      }        
      else if(KeysEqual(key, KEY_3)) {

        if(PINB & 1<<7) {
          PORTB &= ~(1<<7);
        } else {
          PORTB |= 1<<7;          
        }
        STATE = RELAY;
      }
      else {
        STATE = RELAY;
      }
      break;
    }
  }
  return 0;
}

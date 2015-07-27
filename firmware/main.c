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

#define STATE_RELAY   0x00
#define STATE_COMMAND 0x01

uint8_t STATE = STATE_RELAY;

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

static bool KeyEquals(key_t key, key_t other) {
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

static void ExecuteCommand(command_t cmd) {
  uint8_t volatile *port = cmd.port == PORT_A ? &PORTB : &PORTC;
  uint8_t volatile *ddr  = cmd.port == PORT_A ? &DDRB : &DDRC;
  uint8_t value;
  uint8_t mask;
  uint8_t offset;
  uint8_t dir;
  
  switch(cmd.action) {

  case ACTION_SET:
    *ddr |= cmd.mask;
    *port &= ~cmd.mask;
    *port |= cmd.data;
    break;
    
  case ACTION_INV:
    value = *port;
    value &= cmd.mask;
    value ^= cmd.mask;
    
    *ddr |= cmd.mask;
    *port &= ~cmd.mask;
    *port |= value;
    break;

  case ACTION_INC:
  case ACTION_DEC:
    mask = cmd.mask;
    value = *port & cmd.mask;
    offset = 0;
    dir = cmd.action == ACTION_INC ? 1 : -1;
    
    while((mask & 1) == 0) {      
      mask = mask >> 1;
      value = value >> 1;
      offset++;
    }
    value += dir;
    value = value & mask;
    value = value << offset;

    *ddr |= cmd.mask;
    *port &= ~cmd.mask;
    *port |= value;
    break;

  case ACTION_TRS:
    *ddr &= ~cmd.mask;
    *port |= cmd.mask;
    break;
  } 
}

//------------------------------------------------------------------------------

int main(void) {

  SetupHardware();

  key_t key = { .col = 0, .row = 0 };
  
  while(true) {
    
    switch(STATE) {

    //========================================

    case STATE_RELAY:
      PORTB &= ~1;

      if(QueryKeyPress(KEY_META)) {
        STATE = STATE_COMMAND;
      }
      else {
        RelayMatrix();
      }
      break;

    //========================================
      
    case STATE_COMMAND:
      PORTB |= 1;

      ReadKeyPress(&key);
      
      if(KeyEquals(key, KEY_META)) {
        RelayKeyPress(KEY_META);
        STATE = STATE_RELAY;
      }

      /* These hardcoded bindings will be replaced by a loop through
       * the table of bindings which will be stored in eeprom...
      */
      else if(KeyEquals(key, KEY_1)) {

        command_t inv7 = {
          .action = ACTION_INV,
          .port = PORT_A,
          .mask = 0x80,
          .data = 0x00
        };
        
        ExecuteCommand(inv7);        
        STATE = STATE_RELAY;
      }

      else if(KeyEquals(key, KEY_2)) {

        command_t set23_10 = {
          .action = ACTION_SET,
          .port = PORT_A,
          .mask = 0b00001100,
          .data = 0b00001000
        };
        
        ExecuteCommand(set23_10);        
        STATE = STATE_RELAY;
      }

      else if(KeyEquals(key, KEY_3)) {

        command_t set23_01 = {
          .action = ACTION_SET,
          .port = PORT_A,
          .mask = 0b00001100,
          .data = 0b00000100
        };
        
        ExecuteCommand(set23_01);        
        STATE = STATE_RELAY;
      }

      else if(KeyEquals(key, KEY_4)) {

        command_t inc56 = {
          .action = ACTION_INC,
          .port = PORT_A,
          .mask = 0b01100000,
          .data = 0b00000000
        };
        
        ExecuteCommand(inc56);        
        STATE = STATE_RELAY;
      }

      else if(KeyEquals(key, KEY_5)) {

        command_t dec56 = {
          .action = ACTION_DEC,
          .port = PORT_A,
          .mask = 0b01100000,
          .data = 0b00000000
        };
        
        ExecuteCommand(dec56);        
        STATE = STATE_RELAY;
      }
      
      else {
        STATE = STATE_RELAY;
      }
      break;

    //========================================
      
    }
  }
  return 0;
}

//------------------------------------------------------------------------------

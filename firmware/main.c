#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"

uint8_t MC = 1<<PD3; // Matrix Clock
uint8_t MD = 1<<PD4; // Matrix Data
uint8_t MR = 1<<PD0; // Matrix Counter Reset

uint8_t CPS = 1<<PA6; // Crosspoint Strobe
uint8_t CPD = 1<<PA7; // Crosspoint Data
uint8_t CPR = 1<<PD7; // Crosspoint Reset

uint8_t CS  = 1<<PD5; // Cassette Sense
uint8_t CD  = 1<<PD6; // Cassette Write

#define STATE_RELAY   0x00
#define STATE_COMMAND 0x01

volatile Config* config;
volatile uint8_t STATE = STATE_RELAY;

volatile bool matrix[64];

volatile uint8_t serialBit  = 1;
volatile uint8_t serialByte = 0;

//------------------------------------------------------------------------------

#include "config.h"
#include "../config.c"

//------------------------------------------------------------------------------

void SetupHardware(void) {

  clock_prescale_set(clock_div_1);

  DisableJTAG();

  // Crosspoint Control
  DDRA  = 0b11111111;
  PORTA = 0b11000000;

  // USB, Bootloader, Matrix, Serial interface
  DDRD  = 0b10001111;
  PORTD = 0b11110110;

  ResetCrosspointSwitch();
  SetupSerial();  
  sei();
}

//------------------------------------------------------------------------------

/* Serial interface at 6510 IO port
 * Cassete Sense is /STROBE (bit 4 of $01) 
 * Cassete Write is DATA    (bit 3 of $01)
 *
 * The C64 sends eight bits to trigger a key/command
 *
 * TODO: set up a timeout to reset state if the C64 does not
 * send a complete byte
 */

ISR(PCINT3_vect) {
  Key key;
  
  if((PIND & CS) == 0) {
    if((PIND & CD) != 0) {
      serialByte |= serialBit;
    }    
    serialBit = serialBit << 1;

    if(!serialBit) {

      Key_set(&key, serialByte);
      ExecuteBinding(&key);
    
      serialBit  = 1;
      serialByte = 0;
    }
  }
}

//------------------------------------------------------------------------------

void SetupSerial(void) {
  PCMSK3 |= CS;
  PCICR |= (1<<PCIE3);
}

//------------------------------------------------------------------------------
  
int ReadEeprom(FILE* file) {

  static volatile uint16_t addr = 0;
  
  while(EECR & (1<<EEPE));
  EEAR = addr;
  EECR |= (1<<EERE);
  addr += 1;

  return EEDR;
}

//------------------------------------------------------------------------------

FILE eeprom = FDEV_SETUP_STREAM(NULL, ReadEeprom, _FDEV_SETUP_READ);

//------------------------------------------------------------------------------

void ApplyConfig(void) {
  for(int i=0; i<config->size; i++) {
    if(Key_equals(&KEY_INIT, config->bindings[i]->key)) {
      for(int k=0; k<config->bindings[i]->size; k++) {
        ExecuteCommand(config->bindings[i]->commands[k]);
      }
    }
  }
}

//------------------------------------------------------------------------------

void DisableJTAG(void) {
  MCUCR |= (1<<JTD);
  MCUCR |= (1<<JTD);
}

//------------------------------------------------------------------------------

void ResetCounter(void) {
  PORTD |= MR;
  PORTD &= ~MR;
}

//------------------------------------------------------------------------------

void ClockMatrix(void) {
  PORTD |= MC;
  _delay_us(5);
  PORTD &= ~MC;
}

//------------------------------------------------------------------------------

bool ScanMatrix(void) {
  bool keyDown = false;
  uint8_t row = 0;
  uint8_t col = 0;  
  
  ResetCounter();
  
  for(row=0; row<8; row++) {
    for(col=0; col<8; col++) {
      if((PIND & MD) == 0) {
        matrix[row*8+col] = true;
        keyDown = true;
      }
      else {
        matrix[row*8+col] = false;
      }
      ClockMatrix();
    }
  }
  return keyDown;
}

//------------------------------------------------------------------------------

void ResetCrosspointSwitch(void) {
  PORTD &= ~CPR;
  _delay_ms(1);
  PORTD |= CPR;
}

void SetCrosspointSwitch(uint8_t index, bool closed) {
  index = closed ? (index | CPD) : (index & ~CPD);
  index |= CPS;
  PORTA = index;
  
  PORTA &= ~CPS;
  PORTA |= CPS;
}

//------------------------------------------------------------------------------

void RelayMatrix(void) {
  for(uint8_t i=0; i<64; i++) {
    SetCrosspointSwitch(i, matrix[i]);
  }
}

//------------------------------------------------------------------------------

void RelayKeyPress(Key key) {
  ResetCrosspointSwitch();
  
  SetCrosspointSwitch(key.row*8+key.col, true);
  _delay_ms(25);
  SetCrosspointSwitch(key.row*8+key.col, false);
} 

//------------------------------------------------------------------------------

bool QueryKeyPress(Key key) {

  /* If the requested key is down, wait until it is released. */
  
  bool result = false;
  
  if(ScanMatrix() && matrix[key.row*8+key.col]) {
    _delay_ms(20);
    if(ScanMatrix() && matrix[key.row*8+key.col]) {
      result = true;
      while(ScanMatrix() && matrix[key.row*8+key.col]);
    }
  }
  return result;
}

//------------------------------------------------------------------------------

void ReadKeyPress(Key* key) {

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
        if(matrix[row*8+col]) {
          key->row = row;
          key->col = col;
          goto waitForKeyRelease;
        }
      }
    }

  waitForKeyRelease:
    while(ScanMatrix() && matrix[key->row*8+key->col]);
  }
}

//------------------------------------------------------------------------------

void ExecuteBinding(Key* key) {
  Binding *binding;

  for(int i=0; i<config->size; i++) {
    binding = config->bindings[i];
    if(Key_equals(key, binding->key)) {    
      for(int k=0; k<binding->size; k++) {
        ExecuteCommand(binding->commands[k]);
      }
    }
  }
}

//------------------------------------------------------------------------------

void ExecuteCommand(Command* cmd) {
  uint8_t volatile *port = (cmd->port == PORT_A) ? &PORTB : &PORTC;
  uint8_t volatile *ddr  = (cmd->port == PORT_A) ? &DDRB : &DDRC;
  uint8_t value;
  uint8_t mask;
  uint8_t offset;
  uint8_t dir;
  Key key;

  switch(cmd->action) {

  case ACTION_NONE:
    break;
    
  case ACTION_SET:
    offset = 0;
    mask = cmd->mask;

    while((mask & 1) == 0) {      
      mask = mask >> 1;
      offset++;
    }

    value = (cmd->data & mask) << offset;
    
    *ddr |= cmd->mask;
    *port &= ~cmd->mask;
    *port |= value; 
    break;

  case ACTION_CLEAR:
    *ddr |= cmd->mask;
    *port &= ~cmd->mask;
    break;
    
  case ACTION_INVERT:
    value = *port;
    value &= cmd->mask;
    value ^= cmd->mask;
    
    *ddr |= cmd->mask;
    *port &= ~cmd->mask;
    *port |= value;
    break;

  case ACTION_INCREASE:
  case ACTION_DECREASE:
    mask = cmd->mask;
    value = *port & cmd->mask;
    offset = 0;
    dir = cmd->action == ACTION_INCREASE ? 1 : -1;
    
    while((mask & 1) == 0) {      
      mask = mask >> 1;
      value = value >> 1;
      offset++;
    }
    value += dir;
    value = value & mask;
    value = value << offset;

    *ddr |= cmd->mask;
    *port &= ~cmd->mask;
    *port |= value;
    break;

  case ACTION_TRISTATE:
    *ddr &= ~cmd->mask;
    *port &= ~cmd->mask;
    break;

  case ACTION_SLEEP:
    value = cmd->data;
    while(value--) {
      _delay_ms(1);
    }
    break;

  case ACTION_EXEC:
    Key_set(&key, cmd->data);
    ExecuteBinding(&key);
    break;
  }
}

//------------------------------------------------------------------------------

int main(void) {

  Key key;
  
  SetupHardware();

  config = Config_new();
  Config_read(config, &eeprom);
  ApplyConfig();

  while(true) {
    switch(STATE) {

    //========================================

    case STATE_RELAY:

      if(QueryKeyPress(KEY_META)) {
        ResetCrosspointSwitch();
        STATE = STATE_COMMAND;
      }
      else {
        RelayMatrix();
      }
      break;

    //========================================
      
    case STATE_COMMAND:

      // TODO:
      // 
      // - Once macros are possible we can simply
      //   bind META-META to a macro typing the meta key
      // 
      // - Think about implementing a timeout
      // 
      // - Think about being able to reflect the states
      //   on a control line -- e.g. a multicolor power led :)
      //
      // Think about an "enter keymap" function to support
      // chained keybindings
      
      ReadKeyPress(&key);
      
      if(Key_equals(&key, &KEY_META)) 
        RelayKeyPress(KEY_META);
      else
        ExecuteBinding(&key);

      STATE = STATE_RELAY;
      break;

    //========================================
      
    }
  }
  return 0;
}

//------------------------------------------------------------------------------

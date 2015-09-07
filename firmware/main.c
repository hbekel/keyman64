#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "encoding.h"

uint8_t MC = 1<<PD3; // Matrix Clock
uint8_t MD = 1<<PD4; // Matrix Data
uint8_t MR = 1<<PD0; // Matrix Counter Reset

uint8_t CPS = 1<<PA6; // Crosspoint Strobe
uint8_t CPD = 1<<PA7; // Crosspoint Data
uint8_t CPR = 1<<PD7; // Crosspoint Reset

uint8_t CS  = 1<<PD6; // Cassette Sense
uint8_t CD  = 1<<PD5; // Cassette Write

#define STATE_RELAY   0x00
#define STATE_COMMAND 0x01
#define DEBOUNCE _delay_ms(20)

volatile uint8_t STATE = STATE_RELAY;

volatile bool matrix[64];

volatile uint8_t serialBit  = 1;
volatile uint8_t serialByte = 0;

volatile Config* config;
volatile uint8_t meta;

uint32_t BootKey ATTR_NO_INIT;

//------------------------------------------------------------------------------

#include "config.h"
#include "../config.c"

//------------------------------------------------------------------------------

void CheckBootloader(void) {
  if((MCUSR & (1 << WDRF)) && (BootKey == MAGIC)) {
    BootKey = 0;
    ((void (*)(void))BOOTLOADER)();
  }
}

//------------------------------------------------------------------------------

void EnterBootloader(void) {
  cli();
  BootKey = MAGIC;
  wdt_enable(WDTO_250MS);
  for(;;);
}

//------------------------------------------------------------------------------

void SetupHardware(void) {

  wdt_disable();
  
  // Crosspoint Control
  DDRA  = 0b11111111;
  PORTA = 0b11000000;

  // USB, Bootloader, Matrix, Serial interface
  DDRD  = 0b10001111;
  PORTD = 0b11110110;

  ResetCrosspointSwitch();
  
  SetupSerial();  
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

void SetupSerial(void) {
  PCMSK3 = CS;
  PCICR |= (1<<PCIE3);
  sei();
}

//------------------------------------------------------------------------------

ISR(PCINT3_vect) {

  if((PIND & CS) == 0) {
    if((PIND & CD) != 0) {
      serialByte |= serialBit;
    }    
    serialBit = serialBit << 1;

    if(!serialBit) {

      ExecuteBinding(serialByte);
    
      serialBit  = 1;
      serialByte = 0;
    }
  }
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
    if(config->bindings[i]->key == KEY_INIT) {
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

//------------------------------------------------------------------------------

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

void RelayKeyPress(volatile uint8_t key) {
  SetCrosspointSwitch(key, true);
  _delay_ms(25);
  SetCrosspointSwitch(key, false);
} 

//------------------------------------------------------------------------------

bool IsKeyDown(volatile uint8_t key) {
  return matrix[key];
}

//------------------------------------------------------------------------------

bool IsKeyUp(volatile uint8_t key) {
  return !matrix[key];
}

//------------------------------------------------------------------------------

bool IsKey(volatile uint8_t key) {
  return key < 64;
}

//------------------------------------------------------------------------------

bool QueryKeyDown(volatile uint8_t key) {
  if(!IsKey(key)) return false;

  bool result = false;
  
  if(ScanMatrix() && IsKeyDown(key)) {
    DEBOUNCE;
    result = ScanMatrix() && IsKeyDown(key);
  }
  return result;
}

//------------------------------------------------------------------------------

bool QueryKeyUp(volatile uint8_t key) {
  if(!IsKey(key)) return false;
  
  bool result = false;

  ScanMatrix();
  
  if(IsKeyUp(key)) {
    DEBOUNCE;
    ScanMatrix();

    result = IsKeyUp(key);
  }
  return result;
}

//------------------------------------------------------------------------------

void Type(char *string) {
  Sequence sequence;
  uint8_t code;
  
  for(int i=0; i<strlen(string); i++) {
    sequence = encoding[(uint8_t)(string[i])];

    for(int k=0; k<sequence.size; k++) {
      code = sequence.codes[k];
      
      if((code & 0xc0U) == 0x80U) {
        SetCrosspointSwitch(code & ~0xc0U, true);
      }
      else if((code & 0xc0U) == 0x40U) {
        SetCrosspointSwitch(code & ~0xc0U, false);
      }
      else if((code & 0xc0U) == 0xc0U) {
        RelayKeyPress(code & ~0xc0U);
      }
    }
  }
}

//------------------------------------------------------------------------------

void ExecuteBinding(uint8_t key) {
  Binding *binding;

  for(int i=0; i<config->size; i++) {
    binding = config->bindings[i];
    if(key == binding->key) {    
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
  uint8_t key;
  uint16_t index;
  
  switch(cmd->action) {
    
  case ACTION_NONE:
    break;

  case ACTION_DEFINE_META:
    meta = cmd->data;
    break;
    
  case ACTION_SET:
    offset = 0;
    mask = cmd->mask;

    while((mask & 1) == 0) {      
      mask = mask >> 1;
      offset++;
    }

    value = (cmd->data & mask) << offset;

    *port &= ~cmd->mask;
    *port |= value;         
    *ddr |= cmd->mask;    
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
    ExecuteBinding(cmd->data);
    break;

  case ACTION_KEY_DOWN:
    SetCrosspointSwitch(cmd->data, true);
    break;

  case ACTION_KEY_UP:
    SetCrosspointSwitch(cmd->data, false);
    break;

  case ACTION_TYPE:
    index = cmd->mask;
    index |= (cmd->data << 8);
    Type(config->strings[index]);
    break;

  case ACTION_BOOT:
    EnterBootloader();
    break;
  }
}

//------------------------------------------------------------------------------

int main(void) {

  SetupHardware();

  meta = KEY_ARROWLEFT;
  
  config = Config_new();
  Config_read(config, &eeprom);

  ApplyConfig();

  Binding *binding;
  bool relayMetaKey = true;

  while(true) {
    switch(STATE) {

    //========================================

    case STATE_RELAY:

      if(QueryKeyDown(meta)) {
        relayMetaKey = true;
        STATE = STATE_COMMAND;	
      }
      else {
        RelayMatrix();
      }
      break;

    //========================================
      
    case STATE_COMMAND:

      if(QueryKeyUp(meta)) {
        if(relayMetaKey) {
          RelayKeyPress(meta);
        }
        STATE = STATE_RELAY;
      }
      else {
        for(int i=0; i<config->size; i++) {
          binding = config->bindings[i];
          
          if(QueryKeyDown(binding->key)) {  
            
            for(int k=0; k<binding->size; k++) {
              ExecuteCommand(binding->commands[k]);
            }
            while(!QueryKeyUp(binding->key));
            relayMetaKey = false;
          }
        }	
      }
      break;
      
    //========================================
      
    }
  }
  return 0;
}

//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "main.h"
#include "encoding.h"
#include "../protocol.h"
#include "../keyboard.h"

uint8_t MD = 1<<PD4; // Matrix Data

uint8_t CPS = 1<<PD3; // Crosspoint Strobe
uint8_t CPD = 1<<PA7; // Crosspoint Data
uint8_t CPR = 1<<PD7; // Crosspoint Reset

uint8_t CLK   = 1<<PD6; // Serial Clock (active low)
uint8_t DATA  = 1<<PD5; // Serial Data

#define STATE_RELAY   0x00
#define STATE_COMMAND 0x01
volatile uint8_t STATE = STATE_RELAY;

#define DEBOUNCE  _delay_ms(20)
#define DELAY     _delay_ms(20)
#define PROPAGATE _delay_ms(1)
#define WITHOUT_DELAY 0
#define SHIFTED 0x40
#define BINDING 0x80

volatile bool matrix[128];
volatile uint8_t locked[128];
volatile uint8_t layout[128];

volatile Serial serial;
volatile Mapping mappings[16];
volatile Config* config;
volatile uint8_t meta;
volatile bool boot;
volatile bool reset;
volatile bool shifted;

static void (*ResetCrosspointSwitch)(void);
static void (*StrobeCrosspointSwitch)(void);

static volatile uint8_t usbCommand;
static volatile uint16_t usbDataReceived;
static volatile uint16_t usbDataLength;
static volatile uint8_t *usbData;
static volatile uint16_t usbDataPos;
static volatile uint16_t usbDelay;

static volatile char version[64];

static volatile uint8_t translated[128];
static volatile uint8_t synonym[128];

//------------------------------------------------------------------------------

#include "config.h"
#include "../config.c"

//------------------------------------------------------------------------------

void EnterBootloader(void) {
  eeprom_write_word((uint16_t *)0x0ffe, (uint16_t) 0xb0b0);
  Reset();
}

void Reset(void) {
  cli();
  wdt_enable(WDTO_250MS);
  for(;;);
}

//------------------------------------------------------------------------------

void SetupHardware(void) {

  wdt_disable();
  
  // Crosspoint Control
  DDRA  = 0b11111111;
  PORTA = 0b00000000;

  // USB, Bootloader, Matrix, Serial interface
  DDRD  = 0b10001001;
  PORTD = 0b11110000;

  SetupSerial();  
}

//------------------------------------------------------------------------------

void SetupTranslation(void) {
  
  for(uint8_t i=0; i<128; i++) {
    translated[i] = KEY_SHIFTRIGHT;
    synonym[i] = 0xff;
  }

  translated[0x39] = KEY_BACKARROW;
  translated[0x3a] = KEY_CONTROL;
  translated[0x3c] = KEY_SHIFTLEFT;
  translated[0x3d] = KEY_RUNSTOP;
  translated[0x64] = KEY_CBM;
  translated[0x58] = KEY_CLRHOME;
  translated[0x6c] = KEY_RETURN;
  translated[0x0d] = KEY_SPACE;
  
  translated[0x38] = KEY_F1;
  translated[0x30] = KEY_F1 | SHIFTED; synonym[0x30] = 0x38;
  translated[0x28] = KEY_F3;
  translated[0x20] = KEY_F3 | SHIFTED; synonym[0x20] = 0x28;  
  translated[0x18] = KEY_F5;
  translated[0x10] = KEY_F5 | SHIFTED; synonym[0x10] = 0x18;    
  translated[0x08] = KEY_F7;
  translated[0x00] = KEY_F7 | SHIFTED; synonym[0x00] = 0x08;      
  
  translated[0x31] = KEY_1;
  translated[0x29] = KEY_2;
  translated[0x21] = KEY_3;
  translated[0x19] = KEY_4;
  translated[0x11] = KEY_5;
  translated[0x12] = KEY_6;
  translated[0x09] = KEY_7;
  translated[0x01] = KEY_8;
  translated[0x79] = KEY_9;
  translated[0x71] = KEY_0;
  translated[0x72] = KEY_PLUS;
  translated[0x69] = KEY_MINUS;
  translated[0x6a] = KEY_POUND;
  translated[0x63] = KEY_INSTDEL;

  translated[0x5c] = KEY_1; synonym[0x5c] = 0x31;
  translated[0x54] = KEY_2; synonym[0x54] = 0x29;
  translated[0x4c] = KEY_3; synonym[0x21] = 0x4c;
  translated[0x5b] = KEY_4; synonym[0x5b] = 0x19;
  translated[0x53] = KEY_5; synonym[0x53] = 0x11;
  translated[0x4b] = KEY_6; synonym[0x4b] = 0x12;
  translated[0x5a] = KEY_7; synonym[0x5a] = 0x09;
  translated[0x52] = KEY_8; synonym[0x52] = 0x01;
  translated[0x4a] = KEY_9; synonym[0x4a] = 0x79;    
  translated[0x5d] = KEY_0; synonym[0x5d] = 0x71;
  translated[0x44] = KEY_RETURN; synonym[0x44] = 0x6c;
  translated[0x41] = KEY_SLASH; synonym[0x41] = 0x7d;
  translated[0x49] = KEY_ASTERISK; synonym[0x49] = 0x6b;
  translated[0x42] = KEY_MINUS; synonym[0x42] = 0x69;
  translated[0x43] = KEY_PLUS; synonym[0x43] = 0x72;
  translated[0x55] = KEY_PERIOD; synonym[0x55] = 0x05;
  
  translated[0x32] = KEY_Q;
  translated[0x2a] = KEY_W;
  translated[0x22] = KEY_E;
  translated[0x1a] = KEY_R;
  translated[0x1b] = KEY_T;
  translated[0x13] = KEY_Y;
  translated[0x0a] = KEY_U;
  translated[0x02] = KEY_I;
  translated[0x7a] = KEY_O;
  translated[0x73] = KEY_P;
  translated[0x74] = KEY_AT;
  translated[0x6b] = KEY_ASTERISK;

  translated[0x33] = KEY_A;
  translated[0x2b] = KEY_S;
  translated[0x23] = KEY_D;
  translated[0x24] = KEY_F;
  translated[0x1c] = KEY_G;
  translated[0x14] = KEY_H;
  translated[0x0b] = KEY_J;
  translated[0x03] = KEY_K;
  translated[0x7b] = KEY_L;
  translated[0x7c] = KEY_COLON;
  translated[0x75] = KEY_SEMICOLON;
  translated[0x6d] = KEY_EQUAL;

  translated[0x34] = KEY_Z;
  translated[0x2c] = KEY_X;
  translated[0x2d] = KEY_C;
  translated[0x25] = KEY_V;
  translated[0x1d] = KEY_B;
  translated[0x15] = KEY_N;
  translated[0x0c] = KEY_M;
  translated[0x04] = KEY_COMMA;
  translated[0x05] = KEY_PERIOD;
  translated[0x7d] = KEY_SLASH;

  translated[0x59] = KEY_UPARROW;
  
  translated[0x68] = KEY_CURSORDOWN;
  translated[0x60] = KEY_CURSORDOWN | SHIFTED; synonym[0x60] = 0x68;
  translated[0x62] = KEY_CURSORLEFT;
  translated[0x61] = KEY_CURSORLEFT | SHIFTED; synonym[0x61] = 0x62;

  translated[0x4d] = 0x80 | BINDING; // NUMPAD 00  
  translated[0x40] = 0x81 | BINDING; // RUN-STOP
  translated[0x78] = 0x82 | BINDING; // F9
  translated[0x70] = 0x83 | BINDING; // F10
  translated[0x50] = 0x84 | BINDING; // OFF/RVS
  translated[0x48] = 0x85 | BINDING; // NORM/GRAPH
  translated[0x51] = 0x86 | BINDING; // NUMPAD CE
}

//------------------------------------------------------------------------------

void SetupSerial(void) {
  ResetSerial();
  
  PCMSK3 = CLK;
  PCICR |= (1<<PCIE3);
  sei();
}

//------------------------------------------------------------------------------

void ResetSerial(void) {
  ExpectNextSerialByte();
  serial.command      = 0;
  serial.arguments[0] = 0;
  serial.arguments[1] = 0;
  serial.index        = 0;
}

//------------------------------------------------------------------------------

void ExpectNextSerialByte() {
  serial.bit  = 1;
  serial.byte = 0;
}

//------------------------------------------------------------------------------

void debug(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  char msg[512];
  vsnprintf(msg, 512, fmt, ap);
  Type(msg);

  va_end(ap);
}

//------------------------------------------------------------------------------

void ExecuteSerialCommand() {

  uint8_t port;
  uint8_t mask;
  uint8_t key;
  
  switch(serial.command) {

  case SERIAL_COMMAND_EXECUTE:
    ExecuteKey(serial.arguments[0]);
    break;

  case SERIAL_COMMAND_KEY_DOWN:
    SetCrosspointSwitchLocked(serial.arguments[0], true, LOCK_SERIAL);
    PROPAGATE;
    break;

  case SERIAL_COMMAND_KEY_UP:
    SetCrosspointSwitchLocked(serial.arguments[0], false, LOCK_SERIAL);
    PROPAGATE;
    break;

  case SERIAL_COMMAND_KEY_PRESS:
    RelayKeyPress(serial.arguments[0]);
    break;

  case SERIAL_COMMAND_MAP:    

    port = ((serial.arguments[1] & 0x8) == 0) ? 0 : 1;
    mask = 1<<(serial.arguments[1] & 0x7);
    key = serial.arguments[0];

    Map(port, mask, key);

    break;
  }  
}

//------------------------------------------------------------------------------

ISR(PCINT3_vect) {

  if((PIND & CLK) != 0) return;

  if((PIND & DATA) != 0) {
    serial.byte |= serial.bit;
  }    
  serial.bit = serial.bit << 1;

  if(serial.bit == 0) { // byte complete
    
    if(serial.index == 0) { // received command 
      serial.command = serial.byte;
      serial.index = SERIAL_COMMAND_ARITY_FOR[serial.command];
    }
    else { // received argument
      serial.arguments[--serial.index] = serial.byte;
    }    

    if(serial.index == 0) { // all arguments received
      ExecuteSerialCommand();
      ResetSerial();
    }
    else {
      ExpectNextSerialByte();
    }
  }
}

//------------------------------------------------------------------------------

void SetupMappings(void) {
  for(uint8_t i=0; i<16; i++) {
    mappings[i].pins = NULL;
  }
}

//------------------------------------------------------------------------------

void Map(uint8_t p, uint8_t mask, uint8_t key) {

  uint8_t volatile *port = (p == PORT_A) ? &PORTB : &PORTC;   
  uint8_t volatile *pins = (p == PORT_A) ? &PINB : &PINC; 
  uint8_t volatile *ddr  = (p == PORT_A) ? &DDRB : &DDRC;

  Mapping volatile *mapping = &mappings[mask | (p<<3)];

  mapping->pins = pins;
  mapping->mask = mask;
  mapping->key = key;
  
  (*ddr) &= ~mask;
  (*port) |= mask;
}

//------------------------------------------------------------------------------

void ApplyMappings(void) {
  bool closed = false;
  Mapping volatile *mapping;
  
  for(uint8_t i=0; i<16; i++) {
    mapping = &mappings[i];
    
    if(mapping->pins == NULL) continue;
    
    closed = ((*mapping->pins) & mapping->mask) == 0;      
    if(closed) {
      SetCrosspointSwitchLocked(mapping->key, true, LOCK_MAP);
    }
    else {
      if(locked[mappings[i].key] == LOCK_MAP) {
        SetCrosspointSwitchLocked(mapping->key, false, LOCK_MAP);
      }
    }
  }
}

//------------------------------------------------------------------------------

void SetupUSB(void) {

  cli();
  usbInit();

  usbDeviceDisconnect();
  
  _delay_ms(500);

  usbDeviceConnect();
  sei();

  usbData = NULL;
}

//------------------------------------------------------------------------------

void SetupKeyboardLayout(void) {
  for(uint8_t i=0; i<128; i++) {
    layout[i] = i;
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

int WriteEeprom(char data, FILE* file) {

  static volatile uint16_t addr = 0;
  
  while(EECR & (1<<EEPE));
  EEAR = addr;
  EEDR = data;
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
  addr += 1;

  return 0;
}

//------------------------------------------------------------------------------

FILE eeprom = FDEV_SETUP_STREAM(WriteEeprom, ReadEeprom, _FDEV_SETUP_RW);

//------------------------------------------------------------------------------

int ReadUSBData(FILE* file) {
  return usbData[usbDataPos++];
}

//------------------------------------------------------------------------------

FILE usbdata = FDEV_SETUP_STREAM(NULL, ReadUSBData, _FDEV_SETUP_READ);

//------------------------------------------------------------------------------

void ExecuteImmediateCommands(volatile Config* cfg, uint16_t delay) {
  uint16_t ms;
  for(int i=0; i<cfg->num_bindings; i++) {
    if(cfg->bindings[i]->key == KEY_IMMEDIATE) {
      for(int k=0; k<cfg->bindings[i]->num_commands; k++) {
        ExecuteCommand(cfg, cfg->bindings[i]->commands[k]);

        if(delay) {
          ms = delay;
          while(ms--) {
            _delay_ms(1);
          }
        }
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

bool ScanMatrix(void) {  
  bool keyDown = false;

  for(uint8_t code=0; code<128; code++) {
    PORTA = code;
    _delay_us(10);
    if((PIND & MD) == 0) {      
      matrix[code] = true;
      keyDown = true;
    }
    else {
      matrix[code] = false;
    }
  }
  for(uint8_t code=0; code<128; code++) {
    if(matrix[code] && (translated[code] & SHIFTED)) {
      matrix[0x3c] = true;
    }
  }
  for(uint8_t code=0; code<128; code++) {
    if(synonym[code] != 0xff) {
      if(matrix[code] || matrix[synonym[code]]) {
        matrix[code] = true;
        matrix[synonym[code]] = true;
      }
    }
  }  
  return keyDown;
}

//------------------------------------------------------------------------------

void ResetCrosspointSwitch22106(void) {
  PORTD &= ~CPR;
  _delay_ms(1);
  PORTD |= CPR;

  for(uint8_t i=0; i<128; i++) SetCrosspointSwitch(i, false);
}

void ResetCrosspointSwitch8808(void) {
  PORTD |= CPR;  
  _delay_ms(1);
  PORTD &= ~CPR;

  for(uint8_t i=0; i<128; i++) SetCrosspointSwitch(i, false);
}

//------------------------------------------------------------------------------

void StrobeCrosspointSwitch22106(void) {
  PORTD &= ~CPS;
  PORTD |= CPS;
}

void StrobeCrosspointSwitch8808(void) {
  PORTD |= CPS;
  PORTD &= ~CPS;
}

//------------------------------------------------------------------------------

void SetCrosspointSwitch(uint8_t index, bool closed) {
  if(locked[index]) return;
  
  index = closed ? (index | CPD) : (index & ~CPD);
  PORTA = index;
  
  StrobeCrosspointSwitch();
}

//------------------------------------------------------------------------------

void SetCrosspointSwitchLocked(uint8_t index, bool closed, uint8_t lock) {
  if(locked[index] && locked[index] != lock) return;
  
  if(!locked[index]) SetCrosspointSwitch(index, closed);
  locked[index] = closed ? lock : LOCK_NONE;
  if(!locked[index]) SetCrosspointSwitch(index, closed);
}

//------------------------------------------------------------------------------

void RelayMatrix(void) {  
  for(uint8_t i=0; i<128; i++) {
    if(translated[i] & BINDING) {
      if(matrix[i]) {
        ExecuteKeyInteractively(translated[i]);
        GetBinding(translated[i])->locked = true;
      }
      else {
        GetBinding(translated[i])->locked = false;
      }
      continue;
    }
    else {
      SetCrosspointSwitch(layout[translated[i] & 0x3f], matrix[i]);
    }
  }
}

//------------------------------------------------------------------------------

void RelayKeyPress(volatile uint8_t key) {
  SetCrosspointSwitch(key, true);
  DELAY;
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
  return key < 128;
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

volatile uint8_t last;

void Type(char *string) {
  Sequence sequence;
  uint8_t code;
  uint8_t key;

  for(int i=0; i<strlen(string); i++) {
    sequence = encoding[(uint8_t)(string[i])];

    for(int k=0; k<sequence.size; k++) {
      code = sequence.codes[k];
      
      if((code & CODE_MASK) == CODE_KEY_DOWN) {
        SetCrosspointSwitch(code & ~CODE_MASK, true);
        PROPAGATE;
      }
      else if((code & CODE_MASK) == CODE_KEY_UP) {
        SetCrosspointSwitch(code & ~CODE_MASK, false);
        PROPAGATE;
      }
      else if((code & CODE_MASK) == CODE_KEY_PRESS) {
        key = (code & ~CODE_MASK);

        if(key == last) DELAY;

        RelayKeyPress(key);
        last = key;
      }
    }
  }
}
 
//------------------------------------------------------------------------------
 
Binding* GetBinding(uint8_t key) {
  Binding *binding;
  
  for(int i=0; i<config->num_bindings; i++) {
    binding = config->bindings[i];
    
    if(key == binding->key) {    
      return binding;
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------
 
void ExecuteKey(uint8_t key) {
  Binding *binding = GetBinding(key);
  if(binding) {
    ExecuteBinding(binding);
  }
}

//------------------------------------------------------------------------------

void ExecuteKeyInteractively(uint8_t key) {
  Binding *binding = GetBinding(key);
  if(binding) {
    ExecuteBindingInteractively(binding);
  }
}

//------------------------------------------------------------------------------

void ExecuteBindingInteractively(Binding *binding) {

  if(binding->count > 0) {
    if(--(binding->count) == 0) {
      ExecuteBinding(binding);
      Binding_reset(binding);
    }
  }
}

//------------------------------------------------------------------------------

void ExecuteBinding(Binding* binding) {
  Command *command;

  if(binding->locked) return;  
  
  for(int i=0; i<binding->num_commands; i++) {
    command = binding->commands[i];
    
    if((command->policy == POLICY_ALWAYS) ||
       (command->policy == POLICY_EVEN && binding->state == STATE_EVEN) ||
       (command->policy == POLICY_ODD  && binding->state == STATE_ODD)) {
      
      ExecuteCommand(config, command);
    }
  }
  binding->state = (binding->state == STATE_EVEN) ? STATE_ODD : STATE_EVEN;
}

//------------------------------------------------------------------------------

void ExecuteCommand(volatile Config *cfg, Command* cmd) {
  uint8_t volatile *port = (cmd->port == PORT_A) ? &PORTB : &PORTC;
  uint8_t volatile *ddr  = (cmd->port == PORT_A) ? &DDRB : &DDRC;
  uint8_t value;
  uint8_t mask;
  uint8_t offset;
  uint8_t dir;
  uint8_t key;
  uint8_t tmp;
  uint16_t index;
  uint32_t duration;
  
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

  case ACTION_SLEEP_SHORT:
    duration = cmd->mask | (cmd->data << 8);
    while(duration--) {
      _delay_ms(1);
    }
    break;

  case ACTION_SLEEP_LONG:
    index = cmd->mask | (cmd->data << 8);
    duration = cfg->longs[index];
    while(duration--) {
      _delay_ms(1);
    }
    break;
    
  case ACTION_EXEC:
    ExecuteKey(cmd->data);
    break;

  case ACTION_KEY_DOWN:
    SetCrosspointSwitch(cmd->data, true);
    break;

  case ACTION_KEY_UP:
    SetCrosspointSwitch(cmd->data, false);
    break;

  case ACTION_TYPE:
    index = cmd->mask | (cmd->data << 8);
    Type(cfg->strings[index]);
    break;

  case ACTION_BOOT:
    boot = true;
    break;
    
  case ACTION_SWAP:
    tmp = layout[cmd->mask];
    layout[cmd->mask] = layout[cmd->data];
    layout[cmd->data] = tmp;
    break;

  case ACTION_KEY_PRESS:
    RelayKeyPress(cmd->data);
    break;

  case ACTION_DEFINE_SWITCH:
    if(cmd->data == SWITCH_22106) {
      ResetCrosspointSwitch = &ResetCrosspointSwitch22106;
      StrobeCrosspointSwitch = &StrobeCrosspointSwitch22106;  
    }
    if(cmd->data == SWITCH_8808) {
      ResetCrosspointSwitch = &ResetCrosspointSwitch8808;
      StrobeCrosspointSwitch = &StrobeCrosspointSwitch8808;  
    }
    break;

  case ACTION_SAVE_STATE:
    SaveState();
    break;

  case ACTION_RESTORE_STATE:
    RestoreState();
    break;

  case ACTION_MAP:
    Map(cmd->port, cmd->mask, cmd->data);
    break;

  case ACTION_SHOW_VERSION:
    Type((char*)version);
    Type("\n\n");
    break;

  case ACTION_SHOW_STATE:
    ShowState();
    break;
  }
}

//------------------------------------------------------------------------------

void SetupVersionString(void) {

  char *v = "custom cbm600 " xstr(VERSION) " " __DATE__ " " __TIME__;
  int len = strlen(v);

  for(uint8_t i=0; i<64; i++) {
    version[i] = '\0';
  }
  uint8_t o=0;
  
  for(uint8_t i=0; i<strlen(v); i++) {
    if(i>1 && v[i-1] == ' ' && v[i] == ' ') continue;
    version[o++] = tolower(v[i]);
  }
}

//------------------------------------------------------------------------------

static char* p2s(char **dst, uint8_t volatile *port, uint8_t volatile *ddr) {

  uint8_t i=0;
  
  for(uint8_t bit=0x80; bit; bit>>=1, i++) {

    if((*ddr) & bit) {
      (*dst)[i] = ((*port) & bit) ? '1' : '0';
    }
    else {
      (*dst)[i] = ((*port) & bit) ? '1' : 'x';
    }
  }
  (*dst)[8] = '\0';
  return (*dst);
}

//------------------------------------------------------------------------------

void ShowState(void) {
  char *state = "00000000";

  Type("a ");
  Type(p2s(&state, &PORTB, &DDRB));
  Type("\n");

  Type("b ");
  Type(p2s(&state, &PORTC, &DDRC)); 
  Type("\n\n");
}

//------------------------------------------------------------------------------
 
void SaveState(void) {
  config->state->ddra  = DDRB;
  config->state->porta = PORTB;
  config->state->ddrb  = DDRC;
  config->state->portb = PORTC;

  eeprom_write_byte((uint8_t *)2, config->state->ddra);
  eeprom_write_byte((uint8_t *)3, config->state->porta);
  eeprom_write_byte((uint8_t *)4, config->state->ddrb);
  eeprom_write_byte((uint8_t *)5, config->state->portb);
}

//------------------------------------------------------------------------------

void RestoreState(void) {
  DDRB  = config->state->ddra;
  PORTB = config->state->porta;
  DDRC  = config->state->ddrb;
  PORTC = config->state->portb;   
}

//------------------------------------------------------------------------------

USB_PUBLIC usbMsgLen_t usbFunctionSetup(uint8_t data[8]) {

  usbRequest_t *usbRequest = (void*) data;
  
  switch(usbRequest->bRequest) {
    
  case KEYMAN64_CTRL:
  case KEYMAN64_FLASH:
    usbCommand = usbRequest->bRequest;
    usbDataLength = usbRequest->wLength.word;
    usbDelay = usbRequest->wValue.word;
    usbDataReceived = 0;
    
    if(usbData == NULL) {
      usbData = (uint8_t*) calloc(1, sizeof(uint8_t) * usbDataLength);
    }
    return USB_NO_MSG;
    break;

  case KEYMAN64_BOOT:
    boot = true;    
    break;
    
  case KEYMAN64_STATE:
    usbMsgPtr = (uchar *) config->state;
    return 4;
    break;

  case KEYMAN64_IDENTIFY:
    usbMsgPtr = (uchar *) version;
    return strlen((const char*)version)+1;
    break;
  }
  return 0;
}

//------------------------------------------------------------------------------

USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len) {

  for(int i=0; usbDataReceived < usbDataLength && i < len; i++, usbDataReceived++) {
    usbData[usbDataReceived] = (uint8_t) data[i];
  }

  if(usbDataReceived < usbDataLength) {
    return 0;
  }
  else {
    if(usbCommand == KEYMAN64_CTRL) {
      ExecuteCommandsFromUSBData();
    }
    else if(usbCommand == KEYMAN64_FLASH) {
      FlashConfigurationFromUSBData();
    }
  }
  
  free((void*)usbData);
  usbData = NULL;
  return 1;
}

//------------------------------------------------------------------------------

void ExecuteCommandsFromUSBData(void) {

  Config *cfg = Config_new();
  usbDataPos = 0;
  
  if(Config_read(cfg, &usbdata)) {
    ExecuteImmediateCommands(cfg, usbDelay);
  }
  Config_free(cfg);
}

//------------------------------------------------------------------------------

void FlashConfigurationFromUSBData(void) {
  
  // Restore the saved state prior to overriding it in flash...
  RestoreState();
  
  if(fwrite((void*)usbData, sizeof(uint8_t), usbDataLength, &eeprom) == usbDataLength) {

    // Save the previously restored state back into eeprom
    SaveState();

    reset = true;
  }
}

//------------------------------------------------------------------------------

int main(void) {

  SetupHardware();
  SetupKeyboardLayout();
  SetupMappings();
  SetupVersionString();
  
  meta = 0x39; // CBM_ESC
  boot = false;
  reset = false;
  
  ResetCrosspointSwitch = &ResetCrosspointSwitch8808;
  StrobeCrosspointSwitch = &StrobeCrosspointSwitch8808;  
  
  config = Config_new();

  Config_read(config, &eeprom) || Config_install_fallback(config);

  ResetCrosspointSwitch();
  
  ExecuteImmediateCommands(config, WITHOUT_DELAY);

  SetupTranslation();
  
  SetupUSB();
  
  Binding *binding;
  bool relayMetaKey = true;
  
  while(true) {

    usbPoll();

    if(boot || reset) {
      _delay_ms(250);
      if(boot) EnterBootloader();
      if(reset) Reset();
    }

    ApplyMappings();


    switch(STATE) {

    //========================================

    case STATE_RELAY:

      if(QueryKeyDown(meta)) {
        Config_reset(config);
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
          RelayKeyPress(translated[meta]);
        }
        STATE = STATE_RELAY;
      }
      else {
        for(int i=0; i<config->num_bindings; i++) {
          binding = config->bindings[i];
          
          if(QueryKeyDown(binding->key)) {  
            
            ExecuteBindingInteractively(binding);

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

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

char *version = xstr(VERSION);

uint8_t MC = 1<<PD3; // Matrix Clock
uint8_t MD = 1<<PD4; // Matrix Data
uint8_t MR = 1<<PD0; // Matrix Counter Reset

uint8_t CPS = 1<<PA6; // Crosspoint Strobe
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

volatile bool matrix[64];
volatile uint8_t locked[64];
volatile uint8_t layout[64];

volatile Serial serial;
volatile Mapping mappings[16];
volatile Config* config;
volatile uint8_t meta;
volatile bool boot;

static void (*ResetCrosspointSwitch)(void);
static void (*StrobeCrosspointSwitch)(void);

static volatile uint16_t usbDataReceived;
static volatile uint16_t usbDataLength;
static volatile uint8_t *usbData;
static volatile uint16_t usbDataPos;
static volatile uint16_t usbDelay;

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
  DDRD  = 0b10001001;
  PORTD = 0b11110000;

  SetupSerial();  
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
  for(uint8_t i=0; i<64; i++) {
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

FILE eeprom = FDEV_SETUP_STREAM(NULL, ReadEeprom, _FDEV_SETUP_READ);

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

void ResetCrosspointSwitch22106(void) {
  PORTD &= ~CPR;
  _delay_ms(1);
  PORTD |= CPR;

  for(uint8_t i=0; i<64; i++) SetCrosspointSwitch(i, false);
}

void ResetCrosspointSwitch8808(void) {
  PORTD |= CPR;  
  _delay_ms(1);
  PORTD &= ~CPR;

  for(uint8_t i=0; i<64; i++) SetCrosspointSwitch(i, false);
}

//------------------------------------------------------------------------------

void StrobeCrosspointSwitch22106(void) {
  PORTA &= ~CPS;
  PORTA |= CPS;
}

void StrobeCrosspointSwitch8808(void) {
  PORTA |= CPS;
  PORTA &= ~CPS;
}

//------------------------------------------------------------------------------

void SetCrosspointSwitch(uint8_t index, bool closed) {
  if(locked[index]) return;

  index = closed ? (index | CPD) : (index & ~CPD);
  index |= CPS;
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
  for(uint8_t i=0; i<64; i++) {
    SetCrosspointSwitch(layout[i], matrix[i]);
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

void ExecuteKey(uint8_t key) {
  Binding *binding;
  
  for(int i=0; i<config->num_bindings; i++) {
    binding = config->bindings[i];
    
    if(key == binding->key) {    
      ExecuteBinding(binding);
    }
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
    ShowVersion();
    break;

  case ACTION_SHOW_STATE:
    ShowState();
    break;
  }
}

//------------------------------------------------------------------------------

void ShowVersion(void) { // FIXME: tolower
  char *version = "firmware " xstr(VERSION) " (" __DATE__ " " __TIME__ ")\n\n";
  int len = strlen(version);

  for(uint8_t i=0; i<len; i++) {
    version[i] = tolower(version[i]);    
  }
  Type(version);
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

  usbRequest_t *request = (void*) data;

  switch(request->bRequest) {
    
  case KEYMAN64_CTRL:
    usbDataLength = request->wLength.word;
    usbDelay = request->wValue.word;
    usbDataReceived = 0;
    
    if(usbData == NULL) {
      usbData = (uint8_t*) calloc(1, sizeof(uint8_t) * usbDataLength);
    }
    return USB_NO_MSG;
    break;

  case KEYMAN64_STATE:
    usbMsgPtr = (uchar *) config->state;
    return 4;
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
    ExecuteCommandsFromUSBData();
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

int main(void) {

  SetupHardware();
  SetupKeyboardLayout();
  SetupMappings();

  meta = KEY_BACKARROW;
  boot = false;
  
  ResetCrosspointSwitch = &ResetCrosspointSwitch8808;
  StrobeCrosspointSwitch = &StrobeCrosspointSwitch8808;  
  
  config = Config_new();

  Config_read(config, &eeprom) || Config_install_fallback(config);

  ResetCrosspointSwitch();
  
  ExecuteImmediateCommands(config, WITHOUT_DELAY);

  SetupUSB();
  
  Binding *binding;
  bool relayMetaKey = true;
  
  while(true) {

    usbPoll();

    if(boot) {
      _delay_ms(250);
      EnterBootloader();
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
          RelayKeyPress(meta);
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

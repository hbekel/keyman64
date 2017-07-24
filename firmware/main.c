#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
#define STATE_LOCKED  0x02
volatile uint8_t STATE = STATE_RELAY;

#define DEBOUNCE  _delay_ms(20)
#define DELAY     _delay_ms(20)
#define PROPAGATE _delay_ms(1)
#define WITHOUT_DELAY 0

volatile bool matrix[64];
volatile uint8_t locked[64];
volatile uint8_t layout[64];

Storage* storage;
State* transient;

volatile Serial serial;
volatile Mapping mappings[16];
volatile Config* config;
volatile uint8_t meta;
volatile bool boot;
volatile bool reset;

static void (*ResetCrosspointSwitch)(void);
static void (*StrobeCrosspointSwitch)(void);
static void (*ClockMatrix)(void);

static volatile uint8_t usbCommand;
static volatile uint16_t usbDataReceived;
static volatile uint16_t usbDataLength;
static volatile uint8_t *usbData;
static volatile uint16_t usbDataPos;
static volatile uint16_t usbDelay;

static volatile char version[64];

//-----------------------------------------------------------------------------

#include "config.h"
#include "../config.c"

//-----------------------------------------------------------------------------

void EnterBootloader(void) {
  storage->bootflag = BOOTLOADER_MAGIC;
  Storage_save_bootflag(storage);
  Reset();
}

void Reset(void) {
  cli();
  wdt_enable(WDTO_250MS);
  for(;;);
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void SetupSerial(void) {
  ResetSerial();
  
  PCMSK3 = CLK;
  PCICR |= (1<<PCIE3);
  sei();
}

//-----------------------------------------------------------------------------

void ResetSerial(void) {
  ExpectNextSerialByte();
  serial.command      = 0;
  serial.arguments[0] = 0;
  serial.arguments[1] = 0;
  serial.index        = 0;
}

//-----------------------------------------------------------------------------

void ExpectNextSerialByte() {
  serial.bit  = 1;
  serial.byte = 0;
}

//-----------------------------------------------------------------------------

void ExecuteSerialCommand() {

  uint8_t port;
  uint8_t mask;
  uint8_t key;
  Command *command;
  
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

  case SERIAL_COMMAND_SET:
    command = Command_new();
    command->port = ((serial.arguments[0] & 0x80) == 0) ? 0 : 1;
    command->mask = 1<<((serial.arguments[0] & 0x70)>>4);
    command->data = serial.arguments[0] & 0xf;

    switch(command->data) {
    case 0 : command->action = ACTION_CLEAR;    break;
    case 1 : command->action = ACTION_SET;      break;
    case 2 : command->action = ACTION_TRISTATE; break;
    default: command->action = ACTION_NONE;     break;
    }

    ExecuteCommand(config, command);    
    break;
    }  
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void SetupMappings(void) {
  for(uint8_t i=0; i<16; i++) {
    mappings[i].pins = NULL;
  }
}

//-----------------------------------------------------------------------------

static uint8_t mask2bitnum(uint8_t mask) {
  for(uint8_t i = 0; i<8; i++) {
    if(mask & (1<<i)) return i;
  }
  return 0;
}

//-----------------------------------------------------------------------------

void Map(uint8_t p, uint8_t mask, uint8_t key) {

  uint8_t volatile *port = (p == PORT_A) ? &PORTB : &PORTC;   
  uint8_t volatile *pins = (p == PORT_A) ? &PINB : &PINC; 
  uint8_t volatile *ddr  = (p == PORT_A) ? &DDRB : &DDRC;

  Mapping volatile *mapping = &mappings[mask2bitnum(mask) | (p<<3)];

  mapping->pins = pins;
  mapping->mask = mask;
  mapping->key = key;
  
  (*ddr) &= ~mask;
  (*port) |= mask;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void SetupUSB(void) {

  cli();
  usbInit();

  usbDeviceDisconnect();
  
  _delay_ms(500);

  usbDeviceConnect();
  sei();

  usbData = NULL;
}

//-----------------------------------------------------------------------------

void SetupKeyboardLayout(void) {
  for(uint8_t i=0; i<64; i++) {
    layout[i] = i;
  }
}

//-----------------------------------------------------------------------------

int ReadEeprom(FILE* file) {

  static volatile uint16_t addr = 0;
  return eeprom_read_byte((uint8_t *) (addr++));
}

//-----------------------------------------------------------------------------

int WriteEeprom(char data, FILE* file) {

  static volatile uint16_t addr = 0;
  eeprom_update_byte((uint8_t*) (addr++), data);
  return 0;
}

//-----------------------------------------------------------------------------

FILE eeprom = FDEV_SETUP_STREAM(WriteEeprom, ReadEeprom, _FDEV_SETUP_RW);

//-----------------------------------------------------------------------------

int ReadUSBData(FILE* file) {
  return usbData[usbDataPos++];
}

//-----------------------------------------------------------------------------

FILE usbdata = FDEV_SETUP_STREAM(NULL, ReadUSBData, _FDEV_SETUP_READ);

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void DisableJTAG(void) {
  MCUCR |= (1<<JTD);
  MCUCR |= (1<<JTD);
}

//-----------------------------------------------------------------------------

void ResetCounter(void) {
  PORTD |= MR;
  PORTD &= ~MR;
}

//-----------------------------------------------------------------------------

void ClockMatrixFast(void) {
  PORTD |= MC;
  _delay_us(5);
  PORTD &= ~MC;
}

//-----------------------------------------------------------------------------

void ClockMatrixSlow(void) {
  PORTD |= MC;
  _delay_us(50);
  PORTD &= ~MC;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void StrobeCrosspointSwitch22106(void) {
  PORTA &= ~CPS;
  PORTA |= CPS;
}

void StrobeCrosspointSwitch8808(void) {
  PORTA |= CPS;
  PORTA &= ~CPS;
}

//-----------------------------------------------------------------------------

void SetCrosspointSwitch(uint8_t index, bool closed) {
  if(locked[index]) return;

  index = closed ? (index | CPD) : (index & ~CPD);
  index |= CPS;
  PORTA = index;
  
  StrobeCrosspointSwitch();
}

//-----------------------------------------------------------------------------

void SetCrosspointSwitchLocked(uint8_t index, bool closed, uint8_t lock) {
  if(locked[index] && locked[index] != lock) return;
  
  if(!locked[index]) SetCrosspointSwitch(index, closed);
  locked[index] = closed ? lock : LOCK_NONE;
  if(!locked[index]) SetCrosspointSwitch(index, closed);
}

//-----------------------------------------------------------------------------

void RelayMatrix(void) {
  for(uint8_t i=0; i<64; i++) {
    SetCrosspointSwitch(layout[i], matrix[i]);
  }
}

//-----------------------------------------------------------------------------

void RelayKeyPress(volatile uint8_t key) {
  SetCrosspointSwitch(key, true);
  DELAY;
  SetCrosspointSwitch(key, false);
} 

//-----------------------------------------------------------------------------

bool IsKeyDown(volatile uint8_t key) {
  return matrix[key];
}

//-----------------------------------------------------------------------------

bool IsKeyUp(volatile uint8_t key) {
  return !matrix[key];
}

//-----------------------------------------------------------------------------

bool IsKey(volatile uint8_t key) {
  return key < 64;
}

//-----------------------------------------------------------------------------

bool QueryKeyDown(volatile uint8_t key) {
  if(!IsKey(key)) return false;

  bool result = false;
  
  if(ScanMatrix() && IsKeyDown(key)) {
    DEBOUNCE;
    result = ScanMatrix() && IsKeyDown(key);
  }
  return result;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

volatile uint8_t last;

void Type(const char *string) {
  Sequence sequence;
  uint8_t code;
  uint8_t key;
  uint16_t len = strlen(string);
  
  for(uint16_t i=0; i<len; i++) {
    sequence = encoding[(uint8_t)(string[i])];

    for(uint16_t k=0; k<sequence.size; k++) {
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

//-----------------------------------------------------------------------------

void ExecuteKey(uint8_t key) {
  Binding *binding;
  
  for(int i=0; i<config->num_bindings; i++) {
    binding = config->bindings[i];
    
    if(key == binding->key) {    
      ExecuteBinding(binding);
    }
  }
}

//-----------------------------------------------------------------------------

void ExecuteBindingInteractively(Binding *binding) {

  if(binding->count > 0) {
    if(--(binding->count) == 0) {
      ExecuteBinding(binding);
      Binding_reset(binding);
    }
  }
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void Lock(void) {

  if(strlen(storage->password)) {
    ExecuteKey(KEY_LOCKED);
    STATE = STATE_LOCKED;
  }
  else {
    Type("no password defined\n\n");
  }
}

//-----------------------------------------------------------------------------

void Unlock(void) {
  ExecuteKey(KEY_UNLOCKED);
  STATE = STATE_RELAY;
}

//-----------------------------------------------------------------------------

bool ResolvePort(uint8_t num, uint8_t volatile **port, uint8_t volatile** ddr) {

  *port = NULL;
  
  if(num <= PORT_B) {
    *port = (num == PORT_A) ? &PORTB : &PORTC;
    *ddr  = (num == PORT_A) ? &DDRB : &DDRC;
  }
  else if(Config_has_expansion(config)) {

    Expansion *expansion = config->expansion;
    num -= 2;    
    
    if(num < expansion->num_ports) {
      *port = &(expansion->ports[num]);
      *ddr  = &(expansion->ddr);
    }
  }
  return *port != NULL;
}

//-----------------------------------------------------------------------------

void ExecuteCommand(volatile Config *cfg, Command* cmd) {
  uint8_t volatile *port;
  uint8_t volatile *ddr;

  uint8_t value;
  uint8_t mask;
  uint8_t offset;
  uint8_t dir;
  uint8_t key;
  uint8_t tmp;
  uint16_t index;
  uint32_t duration;

  if(!ResolvePort(cmd->port, &port, &ddr)) return;
  
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

  case ACTION_DEFINE_SPEED:
    if(cmd->data == SPEED_SLOW) {
      ClockMatrix = &ClockMatrixSlow;
    }
    if(cmd->data == SPEED_FAST) {
      ClockMatrix = &ClockMatrixFast;
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
    Type((const char*)version);
    Type("\n\n");
    break;

  case ACTION_SHOW_STATE:
    ShowState();
    break;

  case ACTION_LOCK:
    Lock();
    break;

  case ACTION_SET_PASSWORD:
    SetPassword();
    break;

  case ACTION_MEMORIZE:
    GetState(transient);
    break;

  case ACTION_RECALL:
    SetState(transient);
    break;
  }

  if(Config_has_expansion(config) && cmd->port > PORT_B) {
    Expansion_update(config->expansion);
  }
}

//-----------------------------------------------------------------------------

void SetupVersionString(void) {

  char *v = "firmware " xstr(VERSION) " " __DATE__ " " __TIME__;
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

//-----------------------------------------------------------------------------

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
  return (*dst);
}

//-----------------------------------------------------------------------------

void ShowState(void) {
  const char *template = "a 00000000\n";
  char *line = (char*) calloc(11, sizeof(char)); 
  strcpy(line, template);
  
  char *state = line+2;
  uint8_t ddr = 0xff;
  
  p2s(&state, &PORTB, &DDRB);
  Type(line);  

  p2s(&state, &PORTC, &DDRC);
  line[0]++;
  Type(line);  
  
  if(Config_has_expansion(config)) {
    Expansion *e = config->expansion;
    for(uint8_t i = 0; i < e->num_ports; i++) {
      p2s(&state, &e->ports[i], &ddr);
      line[0]++;
      Type(line);        
    }
  }  
  Type("\n");
  free(line);
}

//-----------------------------------------------------------------------------

void GetState(State* state) {
  uint8_t volatile *port;
  uint8_t volatile *ddr;

  if(Config_has_expansion(config)) {
    for(uint8_t i=0; i<config->expansion->num_ports; i++) {
      if(ResolvePort(i+2, &port, &ddr)) {
        state->ports[i] = (*port);
      }
    }
  }

  state->ddra  = DDRB;
  state->porta = PORTB;
  state->ddrb  = DDRC;
  state->portb = PORTC;  
}

//-----------------------------------------------------------------------------

void SetState(State* state) {
  uint8_t volatile *port;
  uint8_t volatile *ddr;

  if(Config_has_expansion(config)) {  
    for(uint8_t i=0; i<config->expansion->num_ports; i++) {
      if(ResolvePort(i+2, &port, &ddr)) {
        (*port) = state->ports[i];
      }
    }
    Expansion_update(config->expansion);
  }
  
  DDRB  = state->ddra;
  PORTB = state->porta;
  DDRC  = state->ddrb;
  PORTC = state->portb;
}

//-----------------------------------------------------------------------------

void SaveState(void) {
  GetState(storage->state);
  Storage_save_state(storage);
}

//-----------------------------------------------------------------------------

void RestoreState(void) {
  Storage_load_state(storage);
  SetState(storage->state);
}

//-----------------------------------------------------------------------------

void SetPassword(void) {
  char password[64] = "";
  char repeated[64] = "";

  EnterPassword("enter new password: ", password);
  EnterPassword("repeat new password: ", repeated);
    
  if(strlen(password) == strlen(repeated) && strcmp(password, repeated) == 0) {

    if(strlen(password) == 1 && password[0] == KEY_RETURN) {
      storage->password[0] = '\0';
    }
    else {
      strncpy(storage->password, password, sizeof(storage->password));
    }
    Storage_save_password(storage);

    Type("ok, password ");
    if(strlen(storage->password)) {
      Type("set\n\n");
    } else {
      Type("cleared\n\n");
    }
  }
  else {
    Type("passwords differ, nothing changed\n\n");
  }
}

//-----------------------------------------------------------------------------

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
    
  case KEYMAN64_IDENTIFY:
    usbMsgPtr = (uchar *) version;
    return strlen((const char*)version)+1;
    break;

  case KEYMAN64_KEY_DOWN:
    SetCrosspointSwitchLocked(usbRequest->wValue.bytes[0], true, LOCK_USB);
    break;
    
  case KEYMAN64_KEY_UP:
    SetCrosspointSwitchLocked(usbRequest->wValue.bytes[0], false, LOCK_USB);
    break;
  }
  return 0;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void ExecuteCommandsFromUSBData(void) {

  Config *cfg = Config_new();
  usbDataPos = 0;
  
  if(Config_read(cfg, &usbdata)) {
    ExecuteImmediateCommands(cfg, usbDelay);
  }
  Config_free(cfg);
}

//-----------------------------------------------------------------------------

void FlashConfigurationFromUSBData(void) {  
  reset = fwrite((void*)usbData, sizeof(uint8_t), usbDataLength, &eeprom) == usbDataLength;
}

//-----------------------------------------------------------------------------

uint8_t ReadKey(void) {
  ScanMatrix();
  for(uint8_t i=0; i<64; i++) {
    if(matrix[i]) {
      while(!QueryKeyUp(i));
      return i;
    }
  }
  return 0xff;
}

//-----------------------------------------------------------------------------

void EnterPassword(const char* prompt, char* buffer) {
  uint8_t key;
  uint8_t len;
  buffer[0] = 0;

  Type(prompt);
  
  while(1) {

    if((key = ReadKey()) == 0xff) continue;
    
    len = strlen(buffer);
    if(len == 63) {
      buffer[0] = 0;
      len = 0;
    }

    buffer[len] = key;
    buffer[len+1] = 0;

    if(key == KEY_RETURN) {
      Type("\n");
      break;
    }
    Type("*");
  }
}

//-----------------------------------------------------------------------------

bool CheckPassword(void) {

  static char input[64] = "";

  uint8_t key;
  uint8_t len;

  if((key = ReadKey()) != 0xff) {
    
    len = strlen(input);
    if(len == sizeof(input)-1) {
      input[0] = 0;
      len = 0;
    }
    
    input[len] = key;
    input[len+1] = 0;
    len = strlen(input);
    
    for(uint8_t i=0; i<len; i++) {
      if(input[i] != storage->password[i]) {
        input[0] = 0;
        return false;
      }
    }
    if(len == strlen(storage->password)) {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------

Storage* Storage_new(void) {
  Storage* self = (Storage*) calloc(1, sizeof(Storage));
  self->state = State_new();
  return self;
}                                   

//-----------------------------------------------------------------------------

bool Storage_valid(Storage* self) {
  return self->magic == STORAGE_MAGIC;
}
  
//-----------------------------------------------------------------------------
  
void Storage_bootstrap(Storage* self) {
  self->magic = STORAGE_MAGIC;
  self->password[0] = 0;
  self->bootflag = 0;
}

//-----------------------------------------------------------------------------

void Storage_load(Storage* self) {
  Storage_load_magic(self);
  
  if(Storage_valid(self)) {
    Storage_load_state(self);
    Storage_load_password(self);
    Storage_load_bootflag(self);
  }
  else {
    Storage_bootstrap(self);
    Storage_save(self);
  }
}

//-----------------------------------------------------------------------------

void Storage_load_magic(Storage* self) {
  uint16_t address = STORAGE_ADDRESS + offsetof(Storage, magic);

  self->magic = eeprom_read_dword((uint32_t*)address);
}

//-----------------------------------------------------------------------------

void Storage_load_state(Storage* self) {
  uint16_t address = STORAGE_ADDRESS + offsetof(Storage, state);

  self->state->ddra  = eeprom_read_byte((uint8_t*)address++);
  self->state->porta = eeprom_read_byte((uint8_t*)address++);
  self->state->ddrb  = eeprom_read_byte((uint8_t*)address++);
  self->state->portb = eeprom_read_byte((uint8_t*)address++);

  if(Config_has_expansion(config)) {
    for(uint8_t i=0; i<config->expansion->num_ports; i++) {
      self->state->ports[i] = eeprom_read_byte((uint8_t*)address++);
    }
  }
}

//-----------------------------------------------------------------------------

void Storage_load_password(Storage* self) {
  uint16_t address = STORAGE_ADDRESS + offsetof(Storage, password);

  eeprom_read_block((void *) self->password,
                    (const void*)address,
                    sizeof(self->password));
}

//-----------------------------------------------------------------------------

void Storage_load_bootflag(Storage* self) {
  uint16_t address = STORAGE_ADDRESS + offsetof(Storage, bootflag);
  eeprom_read_word((uint16_t*) self->bootflag);
}

//-----------------------------------------------------------------------------

void Storage_save(Storage* self) {
  Storage_save_magic(self);
  Storage_save_state(self);
  Storage_save_password(self);
  Storage_save_bootflag(self);
}

//-----------------------------------------------------------------------------

void Storage_save_magic(Storage* self) {
  uint16_t address = STORAGE_ADDRESS + offsetof(Storage, magic);

  eeprom_update_dword((uint32_t*)address, STORAGE_MAGIC);
}

//-----------------------------------------------------------------------------

void Storage_save_state(Storage* self) {
  uint16_t address = STORAGE_ADDRESS + offsetof(Storage, state);
  
  eeprom_update_byte((uint8_t*)address++, self->state->ddra);
  eeprom_update_byte((uint8_t*)address++, self->state->porta);
  eeprom_update_byte((uint8_t*)address++, self->state->ddrb);
  eeprom_update_byte((uint8_t*)address++, self->state->portb);

  if(Config_has_expansion(config)) {
    for(uint8_t i=0; i<config->expansion->num_ports; i++) {
      eeprom_update_byte((uint8_t*)address++, self->state->ports[i]);
    }
  }
}

//-----------------------------------------------------------------------------

void Storage_save_password(Storage* self) {
  uint16_t address = STORAGE_ADDRESS + offsetof(Storage, password);

  eeprom_update_block((const void*)self->password,
                      (void*)address,
                      sizeof(self->password));  
}

//-----------------------------------------------------------------------------

void Storage_save_bootflag(Storage* self) {
 uint16_t address = STORAGE_ADDRESS + offsetof(Storage, bootflag);

 eeprom_update_word((uint16_t*)address, self->bootflag);
}

//-----------------------------------------------------------------------------

void Storage_free(Storage* self) {
  State_free(self->state);
  free(self);
}

//-----------------------------------------------------------------------------

void SetupExpansion(void) {
  if(Config_has_expansion(config)) {
    Expansion_init(config->expansion);
  }
}

//-----------------------------------------------------------------------------

void Expansion_set(uint8_t pin, bool high) {
  uint8_t volatile *port = ((pin >> 3) == PORT_A) ? &PORTB : &PORTC;
  uint8_t volatile *ddr  = ((pin >> 3) == PORT_A) ? &DDRB : &DDRC;
  
  uint8_t mask = 1 << (pin & 0x07);

  if(high) {
    *port |= mask;
  }
  else {
    *port &= ~mask;
  }
  *ddr |= mask;
}

//-----------------------------------------------------------------------------

void Expansion_init(Expansion* self) {
  Expansion_set(self->enable, HIGH);
  Expansion_set(self->clock, HIGH);
  Expansion_set(self->latch, HIGH);
}

//-----------------------------------------------------------------------------

void Expansion_enable(Expansion* self) {
  Expansion_set(self->enable, LOW);
}

//-----------------------------------------------------------------------------

void Expansion_clock(Expansion* self) {
  Expansion_set(self->clock, LOW);
  Expansion_set(self->clock, HIGH);
}

//-----------------------------------------------------------------------------

void Expansion_latch(Expansion* self) {
  Expansion_set(self->latch, LOW);
  Expansion_set(self->latch, HIGH);
}

//-----------------------------------------------------------------------------

void Expansion_data(Expansion* self, bool high) {
  Expansion_set(self->data, high);
}

//-----------------------------------------------------------------------------

void Expansion_update(Expansion* self) {
  
  uint8_t bitmask;
  uint8_t value;
  
  for(int16_t i=self->num_ports-1; i >= 0; i--) {

    value = self->ports[i];
    bitmask = 0x80;

    do {
      Expansion_data(self, value & bitmask);
      Expansion_clock(self);
    }
    while(bitmask >>= 1);
  }
  Expansion_latch(self);
  Expansion_enable(self);
}

//-----------------------------------------------------------------------------

int main(void) {

  SetupHardware();
  SetupKeyboardLayout();
  SetupMappings();
  SetupVersionString();
  
  meta = KEY_BACKARROW;
  boot = false;
  reset = false;
  
  ResetCrosspointSwitch = &ResetCrosspointSwitch8808;
  StrobeCrosspointSwitch = &StrobeCrosspointSwitch8808;
  ClockMatrix = &ClockMatrixFast;
  
  config = Config_new();

  Config_read(config, &eeprom) || Config_install_fallback(config);

  ResetCrosspointSwitch();

  ScanMatrix();
  RelayMatrix();

  SetupExpansion();

  storage = Storage_new();
  Storage_load(storage);

  transient = State_new();
  GetState(transient);
  
  ExecuteImmediateCommands(config, WITHOUT_DELAY);

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

    case STATE_LOCKED:
      if(CheckPassword()) {
        Unlock();
      }
      break;
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdbool.h>

#define ACTION_SET           0
#define ACTION_INVERT        1
#define ACTION_INCREASE      2
#define ACTION_DECREASE      3
#define ACTION_TRISTATE      4
#define ACTION_SLEEP_SHORT   5
#define ACTION_EXEC          6
#define ACTION_CLEAR         7
#define ACTION_NONE          8
#define ACTION_DEFINE_META   9
#define ACTION_KEY_DOWN      10
#define ACTION_KEY_UP        11
#define ACTION_TYPE          12
#define ACTION_BOOT          13
#define ACTION_SWAP          14
#define ACTION_KEY_PRESS     15
#define ACTION_SLEEP_LONG    16
#define ACTION_DEFINE_SWITCH 17
#define ACTION_SAVE_STATE    18
#define ACTION_RESTORE_STATE 19
#define ACTION_REQUIRES      20
#define ACTION_MAP           21
#define ACTION_SHOW_VERSION  22
#define ACTION_SHOW_STATE    23
#define ACTION_LOCK          24
#define ACTION_SET_PASSWORD  25
#define ACTION_MEMORIZE      26
#define ACTION_RECALL        27
#define ACTION_DEFINE_SPEED  28
#define ACTION_EXPAND        29

#define PORT_A    0
#define PORT_B    1
#define PORT_NONE 0xff

#define SWITCH_22106 0
#define SWITCH_8808  1

#define SPEED_SLOW 0
#define SPEED_FAST 1

const uint8_t POLICY_ALWAYS  = 0b00000000;
const uint8_t POLICY_EVEN    = 0b01000000;
const uint8_t POLICY_ODD     = 0b01100000;

const uint8_t STATE_EVEN     = 0b01000000;
const uint8_t STATE_ODD      = 0b01100000;

typedef struct {
  uint8_t action;
  uint8_t port;
  uint8_t mask;
  uint8_t data;
  uint8_t policy;
} Command;

typedef struct {
  uint8_t num_commands;
  uint8_t key;
  uint8_t state;
  uint8_t times;
  uint8_t count;  
  Command** commands;
} Binding;

typedef struct {
  uint8_t ddra;
  uint8_t porta;
  uint8_t ddrb;
  uint8_t portb;
  uint8_t ports[24];  
} State;

typedef struct {
  uint8_t clock;
  uint8_t data;
  uint8_t latch;
  uint8_t enable;
  uint8_t num_ports;
  uint8_t* ports;
  uint8_t ddr;
} Expansion;

typedef struct {
  uint16_t num_bindings;
  uint16_t num_strings;
  uint16_t num_longs;
  Binding **bindings;
  char **strings;
  uint32_t *longs;
  Expansion *expansion;
} Config;

uint8_t CONFIG_MAGIC[2] = { 0x1c, 0xcf };
uint8_t KEY_EOF       = 0xff;
uint8_t KEY_IMMEDIATE = 0xfe;
uint8_t KEY_STRING    = 0xfd;
uint8_t KEY_LONG      = 0xfc;
uint8_t KEY_EXPANSION = 0xfb;

Config* Config_new(void);
Binding* Config_add_binding(volatile Config *self, Binding* binding);
bool Config_has_binding(volatile Config* self, uint8_t key);
Binding* Config_get_binding(volatile Config* self, uint8_t key);
Binding* Config_get_or_create_binding(volatile Config* self, uint8_t key);
bool Config_has_string(volatile Config *self, char* string, uint16_t *index);
uint16_t Config_add_string(volatile Config *self, char* string);
bool Config_has_long(volatile Config *self, uint32_t value, uint16_t *index);
uint16_t Config_add_long(volatile Config *self, uint32_t value);
Expansion* Config_set_expansion(volatile Config *self, Expansion* expansion);
bool Config_read(volatile Config *self, FILE* in);
bool Config_install_fallback(volatile Config *self);
void Config_reset(volatile Config *self);
bool Config_has_expansion(volatile Config *self);
void Config_free(Config *self);

Binding* Binding_new(void);
void Binding_set_key(Binding* self, uint8_t key);
Command* Binding_add(Binding* self, Command* command);
void Binding_read(Binding *self, FILE* in);
void Binding_reset(Binding *self);
void Binding_free(Binding *self);

Command* Command_new(void);
void Command_read(Command *self, FILE* in);
void Command_free(Command *self);

State* State_new(void);
void State_free(State* self);

Expansion * Expansion_new(void);
void Expansion_set_num_ports(Expansion* self, uint8_t num_ports);
void Expansion_read(Expansion* self, FILE* in);
void Expansion_free(Expansion* self);

#endif // CONFIG_H

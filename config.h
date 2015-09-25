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

#define PORT_A    0
#define PORT_B    1
#define PORT_NONE 2

#define SWITCH_22106 0
#define SWITCH_8808  1

typedef struct {
  uint8_t action;
  uint8_t port;
  uint8_t mask;
  uint8_t data;
} Command;

typedef struct {
  uint8_t size;
  uint8_t key;
  Command** commands;
} Binding;

typedef struct {
  uint16_t size;
  uint16_t _size;
  uint16_t __size;
  Binding **bindings;
  char **strings;
  uint32_t *longs;
} Config;

uint8_t CONFIG_MAGIC[2] = { 0x1c, 0xcf };
uint8_t KEY_EOF    = 0xff;
uint8_t KEY_INIT   = 0xfe;
uint8_t KEY_STRING = 0xfd;
uint8_t KEY_LONG   = 0xfc;

Config* Config_new(void);
Binding* Config_add_binding(volatile Config *self, Binding* binding);
bool Config_has_binding(volatile Config* self, uint8_t key);
Binding* Config_get_binding(volatile Config* self, uint8_t key);
bool Config_has_string(volatile Config *self, char* string, uint16_t *index);
uint16_t Config_add_string(volatile Config *self, char* string);
bool Config_has_long(volatile Config *self, uint32_t value, uint16_t *index);
uint16_t Config_add_long(volatile Config *self, uint32_t value);
bool Config_read(volatile Config *self, FILE* in);

Binding* Binding_new(void);
void Binding_set_key(Binding* self, uint8_t key);
Command* Binding_add(Binding* self, Command* command);
void Binding_read(Binding *self, FILE* in);

Command* Command_new(void);
void Command_read(Command *self, FILE* in);

#endif // CONFIG_H

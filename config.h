#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdbool.h>

#define ACTION_SET         0
#define ACTION_INVERT      1
#define ACTION_INCREASE    2
#define ACTION_DECREASE    3
#define ACTION_TRISTATE    4
#define ACTION_SLEEP       5
#define ACTION_EXEC        6
#define ACTION_CLEAR       7
#define ACTION_NONE        8
#define ACTION_DEFINE_META 9

#define PORT_A    0
#define PORT_B    1
#define PORT_NONE 2

typedef struct {
  uint8_t col;
  uint8_t row;
} Key;

typedef struct {
  uint8_t action;
  uint8_t port;
  uint8_t mask;
  uint8_t data;
} Command;

typedef struct {
  uint8_t size;
  Key* key;
  Command** commands;
} Binding;

typedef struct {
  uint16_t size;
  Binding **bindings;
} Config;

uint8_t CONFIG_MAGIC[2] = { 0x1c, 0xcf };
Key KEY_INIT = { .col = 15, .row = 14 };

Config* Config_new(void);
Binding* Config_add(volatile Config *self, Binding* binding);
bool Config_has_binding(volatile Config* self, Key* key);
Binding* Config_get_binding(volatile Config* self, Key* key);
bool Config_read(volatile Config *self, FILE* in);

Binding* Binding_new(void);
void Binding_set_key(Binding* self, Key* key);
Command* Binding_add(Binding* self, Command* command);
void Binding_read(Binding *self, FILE* in);

Key* Key_new(void);
Key* Key_clone(Key* key);
void Key_set(volatile Key* self, uint8_t byte);
uint8_t Key_get(volatile Key* self);
bool Key_equals(Key* self, Key* key);

Command* Command_new(void);
void Command_read(Command *self, FILE* in);

#endif // CONFIG_H

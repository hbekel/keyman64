#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define ACTION_SET      0
#define ACTION_INVERT   1
#define ACTION_INCREASE 2
#define ACTION_DECREASE 3
#define ACTION_TRISTATE 4
#define ACTION_SLEEP    5
#define ACTION_EXEC     6

#define PORT_A 0
#define PORT_B 1

typedef struct {
  uint8_t col;
  uint8_t row;
} key_t;

typedef struct {
  uint8_t action;
  uint8_t port;
  uint8_t mask;
  uint8_t data;
} command_t;

typedef struct {
  uint16_t size;
  key_t* key;
  command_t** commands;
} binding_t;

typedef struct {
  uint16_t size;
  binding_t **bindings;
} config_t;

static uint8_t ReadEprom(uint16_t addr);
static void ReadConfig(void);
static void ReadBinding(binding_t** binding, uint16_t* addr);
static void ReadKey(key_t* key, uint16_t* addr);
static void ReadCommand(command_t* command, uint16_t* addr);

static void ByteToKey(uint8_t byte, key_t* key);
static uint8_t KeyToByte(key_t key);
static bool KeyEquals(key_t key, key_t other);

#endif // CONFIG_H

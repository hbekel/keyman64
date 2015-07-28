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
} Key;

typedef struct {
  uint8_t action;
  uint8_t port;
  uint8_t mask;
  uint8_t data;
} Command;

typedef struct {
  uint16_t size;
  Key* key;
  Command** commands;
} Binding;

typedef struct {
  uint16_t size;
  Binding **bindings;
} Config;

static uint8_t ReadEprom(uint16_t addr);
static void ReadConfig(void);
static void ReadBinding(Binding** binding, uint16_t* addr);
static void ReadKey(Key* key, uint16_t* addr);
static void ReadCommand(Command* command, uint16_t* addr);

static void ByteToKey(uint8_t byte, Key* key);
static uint8_t KeyToByte(Key key);
static bool KeyEquals(Key key, Key other);

#endif // CONFIG_H

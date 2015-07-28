#include "config.h"

//------------------------------------------------------------------------------

static void ReadConfig(void) {
  uint16_t addr = 0UL;
  uint8_t byte = 0;
  Binding* binding;

  config = (Config*) calloc(1, sizeof(Config));
  config->bindings = (Binding**) calloc(1, sizeof(Binding**));
  config->size = 0;
  
  while((byte = ReadEprom(addr)) != 0xffU) {    
    config->bindings = (Binding**) realloc(config->bindings, (config->size+1)*sizeof(Binding*));
    binding = (Binding*) calloc(1, sizeof(Binding));
    binding->key = (Key*) calloc(1, sizeof(Key));
    binding->commands = (Command**) calloc(1, sizeof(Command**));    
    ReadBinding(&binding, &addr);
    config->bindings[config->size] = binding;
    config->size++;
  }
}

//------------------------------------------------------------------------------

static void ReadBinding(Binding** binding, uint16_t* addr) {
  Command* command;
  ReadKey((*binding)->key, addr);
  (*binding)->size = ReadEprom(*addr); (*(addr))++;
  (*binding)->commands = (Command**) realloc((*binding)->commands, (*binding)->size*sizeof(Command*));
  
  for(int i=0; i<(*binding)->size; i++) {
    command = (Command*) calloc(1, sizeof(Command));
    ReadCommand(command, addr);
    (*binding)->commands[i] = command;
  }
}

//------------------------------------------------------------------------------

static void ReadKey(Key* key, uint16_t* addr) {
  ByteToKey(ReadEprom(*addr), key); (*(addr))++;
}

//------------------------------------------------------------------------------

static void ReadCommand(Command* command, uint16_t* addr) {
  command->action = ReadEprom(*addr); (*(addr))++;
  command->port   = ((command->action & 0x80) == 0) ? 0 : 1;
  command->action &= 0x7f;
  command->mask   = ReadEprom(*addr); (*(addr))++;
  command->data   = ReadEprom(*addr); (*(addr))++;
}

//------------------------------------------------------------------------------

static bool KeyEquals(Key key, Key other) {
  return key.col == other.col && key.row == other.row;
}

//------------------------------------------------------------------------------

static void ByteToKey(uint8_t byte, Key *key) {
  key->col = (byte & 0xf0) >> 4;
  key->row = (byte & 0x0f);
}

//------------------------------------------------------------------------------

static uint8_t KeyToByte(Key key) {
  uint8_t byte = 0;
  byte |= key.row << 4;
  byte |= key.col;
  return byte;
}

//------------------------------------------------------------------------------

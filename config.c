#include "config.h"

static volatile Config *config; //KILLME after refactoring Read* functions

//------------------------------------------------------------------------------

static Config* Config_new(void) {
  Config* self = (Config*) calloc(1, sizeof(Config));
  self->bindings = (Binding**) calloc(1, sizeof(Binding**));
  self->size =0;
  return self;
}

//------------------------------------------------------------------------------

static void Config_add(Config *self, Binding* binding) {
  self->bindings = (Binding**) realloc(self->bindings, (self->size+1)*sizeof(Binding**));
  self->bindings[self->size] = binding;
  self->size++;
}

//------------------------------------------------------------------------------

static Binding* Config_get_binding(Config* self, Key* key) {
  for(int i=0; i<self->size; i++) {
    if(Key_equals(self->bindings[i]->key, key)) {
      return self->bindings[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------

static bool Config_has_binding(Config* self, Key* key) {
  return Config_get_binding(self, key) == NULL;
}

//------------------------------------------------------------------------------

static Binding* Binding_new(void) {
  Binding* self = (Binding*) calloc(1, sizeof(Binding));
  self->commands = (Command**) calloc(1, sizeof(Command**));
  self->key = Key_clone(&KEY_INIT);
  return self;
}

//------------------------------------------------------------------------------

static void Binding_set_key(Binding* self, Key* key) {
  self->key->col = key->col;
  self->key->row = key->row;
}

//------------------------------------------------------------------------------

static void Binding_add(Binding* self, Command* command) {
  self->commands = (Command**) realloc(self->commands, (self->size+1)*sizeof(Command**));
  self->commands[self->size] = command;
  self->size++;
}

//------------------------------------------------------------------------------

static Key* Key_new(void) {
  Key* self = (Key*) calloc(1, sizeof(Key));
  return self;
}

//------------------------------------------------------------------------------

static Key* Key_clone(Key* key) {
  Key* self = Key_new();
  self->col = key->col;
  self->row = key->row;
  return self;
}

//------------------------------------------------------------------------------

static void Key_set(Key* self, uint8_t byte) {
  self->col = (byte & 0xf0) >> 4;
  self->row = (byte & 0x0f);
}

//------------------------------------------------------------------------------

static uint8_t Key_get(Key* self) {
  uint8_t byte = 0;
  byte |= self->col << 4;
  byte |= self->row;
  return byte;
}

//------------------------------------------------------------------------------

static bool Key_equals(Key* self, Key* key) {
  return self->col == key->col && self->row == key->row;
}

//------------------------------------------------------------------------------

static Command* Command_new(void) {
  Command* self = (Command*) calloc(1, sizeof(Command));
  self->action = ACTION_NONE;
  self->port = PORT_A;
  self->mask = 0xff;
  self->data = 0xff;
  return self;
}

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

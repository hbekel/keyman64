#include "config.h"

//------------------------------------------------------------------------------

Config* Config_new(void) {
  Config* self = (Config*) calloc(1, sizeof(Config));
  self->bindings = (Binding**) calloc(1, sizeof(Binding**));
  self->size =0;
  return self;
}

//------------------------------------------------------------------------------

Binding* Config_add(volatile Config *self, Binding* binding) {
  self->bindings = (Binding**) realloc(self->bindings, (self->size+1)*sizeof(Binding**));
  self->bindings[self->size] = binding;
  self->size++;
  return binding;
}

//------------------------------------------------------------------------------

Binding* Config_get_binding(volatile Config* self, Key* key) {
  for(int i=0; i<self->size; i++) {
    if(Key_equals(self->bindings[i]->key, key)) {
      return self->bindings[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------

bool Config_has_binding(volatile Config* self, Key* key) {
  return Config_get_binding(self, key) == NULL;
}

//------------------------------------------------------------------------------

bool Config_read(volatile Config *self, FILE* in) {
  uint8_t byte;
  Binding* binding;

  if(!(fgetc(in) == CONFIG_MAGIC[0] &&
       fgetc(in) == CONFIG_MAGIC[1])) {
    return false;
  }
  
  while((byte = fgetc(in)) != 0xFFU) {
    binding = Config_add(self, Binding_new());
    Key_set(binding->key, byte);    
    Binding_read(binding, in);
  }
  return true;
}

//------------------------------------------------------------------------------

Binding* Binding_new(void) {
  Binding* self = (Binding*) calloc(1, sizeof(Binding));
  self->commands = (Command**) calloc(1, sizeof(Command**));
  self->key = Key_clone(&KEY_INIT);
  return self;
}

//------------------------------------------------------------------------------

void Binding_set_key(Binding* self, Key* key) {
  self->key->col = key->col;
  self->key->row = key->row;
}

//------------------------------------------------------------------------------

Command* Binding_add(Binding* self, Command* command) {
  self->commands = (Command**) realloc(self->commands, (self->size+1)*sizeof(Command**));
  self->commands[self->size] = command;
  self->size++;
  return command;
}

//------------------------------------------------------------------------------

void Binding_read(Binding *self, FILE* in) {
  Command *command;
  uint8_t size = fgetc(in);
  
  for(int i=0; i<size; i++) {
    command = Binding_add(self, Command_new());
    Command_read(command, in);
  }
}

//------------------------------------------------------------------------------

Key* Key_new(void) {
  Key* self = (Key*) calloc(1, sizeof(Key));
  return self;
}

//------------------------------------------------------------------------------

Key* Key_clone(Key* key) {
  Key* self = Key_new();
  self->col = key->col;
  self->row = key->row;
  return self;
}

//------------------------------------------------------------------------------

void Key_set(volatile Key* self, uint8_t byte) {
  self->col = (byte & 0xf0) >> 4;
  self->row = (byte & 0x0f);
}

//------------------------------------------------------------------------------

uint8_t Key_get(Key* self) {
  uint8_t byte = 0;
  byte |= self->col << 4;
  byte |= self->row;
  return byte;
}

//------------------------------------------------------------------------------

bool Key_equals(Key* self, Key* key) {
  return self->col == key->col && self->row == key->row;
}

//------------------------------------------------------------------------------

Command* Command_new(void) {
  Command* self = (Command*) calloc(1, sizeof(Command));
  self->action = ACTION_NONE;
  self->port = PORT_A;
  self->mask = 0xff;
  self->data = 0xff;
  return self;
}

//------------------------------------------------------------------------------

void Command_read(Command *self, FILE* in) {
  self->action = fgetc(in);
  self->port   = (self->action & 0x80) == 0 ? 0 : 1;
  self->action &= 0x7fU;
  self->mask   = fgetc(in);
  self->data   = fgetc(in);
}

//------------------------------------------------------------------------------


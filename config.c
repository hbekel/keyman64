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

Binding* Config_get_binding(volatile Config* self, uint8_t key) {
  for(int i=0; i<self->size; i++) {
    if(key == self->bindings[i]->key) {
      return self->bindings[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------

bool Config_has_binding(volatile Config* self, uint8_t key) {
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
    binding->key = byte;
    Binding_read(binding, in);
  }
  return true;
}

//------------------------------------------------------------------------------

Binding* Binding_new(void) {
  Binding* self = (Binding*) calloc(1, sizeof(Binding));
  self->commands = (Command**) calloc(1, sizeof(Command**));
  self->key = KEY_INIT;
  return self;
}

//------------------------------------------------------------------------------

void Binding_set_key(Binding* self, uint8_t key) {
  self->key = key;
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


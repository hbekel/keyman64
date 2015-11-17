#include <string.h>

#include "config.h"
//------------------------------------------------------------------------------

Config* Config_new(void) {
  Config* self = (Config*) calloc(1, sizeof(Config));
  self->bindings = (Binding**) calloc(1, sizeof(Binding**));
  self->strings = (char**) NULL;
  self->longs = (uint32_t*) NULL;
  self->size = 0;
  self->_size = 0;
  self->__size = 0;
  return self;
}

//------------------------------------------------------------------------------

bool Config_has_string(volatile Config *self, char* string, uint16_t *index) {

  for(uint16_t i=0; i<self->_size; i++) {    
    if(strcmp(self->strings[i], string) == 0) {
      *index = i;
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------

uint16_t Config_add_string(volatile Config *self, char* string) {
  self->strings = (char**) realloc(self->strings, (self->_size+1) * sizeof(char *));
  self->strings[self->_size] = calloc(strlen(string)+1, sizeof(char));
  strncpy(self->strings[self->_size], string, strlen(string));
  self->_size++;
  return self->_size-1;
}

bool Config_has_long(volatile Config *self, uint32_t value, uint16_t *index) {
  for(uint16_t i=0; i<self->__size; i++) {
    if(self->longs[i] == value) {
      *index = i;
      return true;
    }
  }
  return false;
}

uint16_t Config_add_long(volatile Config *self, uint32_t value) {
  self->longs = (uint32_t*) realloc(self->longs, (self->__size+1) * sizeof(uint32_t));
  self->longs[self->__size] = value;
  self->__size++;
  return self->__size-1;
}

//------------------------------------------------------------------------------

Binding* Config_add_binding(volatile Config *self, Binding* binding) {
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
  char string[4096];
  char c;
  uint32_t value;
  int i;

  if(!((c = fgetc(in)) == CONFIG_MAGIC[0])) {
    ungetc(c, in);
    return false;
  }

  if(!((c = fgetc(in)) == CONFIG_MAGIC[1])) {
    ungetc(c, in);
    return false;
  }
  
  while((byte = fgetc(in)) != KEY_EOF) {

    if(byte == KEY_STRING) {
      i = 0;
      while((c = fgetc(in)) != '\0') {
        string[i++] = c;
      }
      string[i] = '\0';
      Config_add_string(self, string);
    }
    else if(byte == KEY_LONG) {
      value = 0;
      value |= ((uint32_t)fgetc(in)) << 0;
      value |= ((uint32_t)fgetc(in)) << 8;
      value |= ((uint32_t)fgetc(in)) << 16;
      value |= ((uint32_t)fgetc(in)) << 24;

      Config_add_long(self, value);
       
    } else {
      binding = Config_add_binding(self, Binding_new());
      binding->key = byte;
      Binding_read(binding, in);
    }
  }
  return true;
}

void Config_free(Config *self) {
  for(int i=0; i<self->size; i++) {
    Binding_free(self->bindings[i]);
  }
  free(self->bindings);

  for(int i=0; i<self->_size; i++) {
    free(self->strings[i]);
  }
  free(self->strings);
  free(self->longs);
  free(self);
}

//------------------------------------------------------------------------------

Binding* Binding_new(void) {
  Binding* self = (Binding*) calloc(1, sizeof(Binding));
  self->commands = (Command**) calloc(1, sizeof(Command**));
  self->key = KEY_IMMEDIATE;
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

void Binding_free(Binding *self) {
  for(int i=0; i<self->size; i++) {
    Command_free(self->commands[i]);
  }
  free(self->commands);
  free(self);
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

void Command_free(Command *self) {
  free(self);
}

//------------------------------------------------------------------------------


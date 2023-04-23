#include <string.h>

#include "config.h"

//-----------------------------------------------------------------------------

Config* Config_new(void) {
  Config* self = (Config*) calloc(1, sizeof(Config));
  self->bindings = (Binding**) calloc(1, sizeof(Binding**));
  self->strings = (char**) NULL;
  self->longs = (uint32_t*) NULL;
  self->expansion = NULL;
  self->num_bindings = 0;
  self->num_strings = 0;
  self->num_longs = 0;
  return self;
}

//-----------------------------------------------------------------------------

bool Config_has_string(volatile Config *self, char* string, uint16_t *index) {

  for(uint16_t i=0; i<self->num_strings; i++) {
    if(strcmp(self->strings[i], string) == 0) {
      *index = i;
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------

uint16_t Config_add_string(volatile Config *self, char* string) {

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstringop-truncation"

  self->strings = (char**) realloc(self->strings, (self->num_strings+1) * sizeof(char *));
  self->strings[self->num_strings] = calloc(strlen(string)+1, sizeof(char));
  strncpy(self->strings[self->num_strings], string, strlen(string));
  self->num_strings++;
  return self->num_strings-1;

  #pragma GCC diagnostic pop
}

//-----------------------------------------------------------------------------

bool Config_has_long(volatile Config *self, uint32_t value, uint16_t *index) {
  for(uint16_t i=0; i<self->num_longs; i++) {
    if(self->longs[i] == value) {
      *index = i;
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------

uint16_t Config_add_long(volatile Config *self, uint32_t value) {
  self->longs = (uint32_t*) realloc(self->longs, (self->num_longs+1) * sizeof(uint32_t));
  self->longs[self->num_longs] = value;
  self->num_longs++;
  return self->num_longs-1;
}

//-----------------------------------------------------------------------------

Binding* Config_add_binding(volatile Config *self, Binding* binding) {
  self->bindings = (Binding**) realloc(self->bindings, (self->num_bindings+1)*sizeof(Binding**));
  self->bindings[self->num_bindings] = binding;
  self->num_bindings++;
  return binding;
}

//-----------------------------------------------------------------------------

Binding* Config_get_binding(volatile Config* self, uint8_t key) {
  for(int i=0; i<self->num_bindings; i++) {
    if(key == self->bindings[i]->key) {
      return self->bindings[i];
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------

bool Config_has_binding(volatile Config* self, uint8_t key) {
  return Config_get_binding(self, key) == NULL;
}

//-----------------------------------------------------------------------------

Binding* Config_get_or_create_binding(volatile Config* self, uint8_t key) {
  Binding* binding = NULL;

  if(!Config_has_binding(self, key)) {
    binding = Config_get_binding(self, key);
  }
  else {
    binding = Binding_new();
    Binding_set_key(binding, key);
    Config_add_binding(self, binding);
  }
  return binding;
}

//-----------------------------------------------------------------------------

bool Config_read(volatile Config *self, FILE* in) {
  uint8_t byte;
  Binding* binding;
  Expansion* expansion;
  char string[4096];
  char c;
  uint32_t value;
  int i;

  if(!(((uint8_t)(c = fgetc(in))) == CONFIG_MAGIC[0])) {
    ungetc(c, in);
    return false;
  }

  if(!(((uint8_t)(c = fgetc(in))) == CONFIG_MAGIC[1])) {
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

    }
    else if(byte == KEY_EXPANSION) {
      expansion = Config_set_expansion(self, Expansion_new());
      Expansion_read(expansion, in);
    }
    else {
      binding = Config_add_binding(self, Binding_new());
      binding->key = byte;
      Binding_read(binding, in);
    }
  }
  return true;
}

//-----------------------------------------------------------------------------

bool Config_has_expansion(volatile Config *self) {
  return self->expansion != NULL;
}

//-----------------------------------------------------------------------------

Expansion* Config_set_expansion(volatile Config *self, Expansion* expansion) {
  return self->expansion = expansion;
}

//-----------------------------------------------------------------------------

#define xstr(s) mstr(s)
#define mstr(s) #s

bool Config_install_fallback(volatile Config *self) {

  Binding *binding;
  Command *command;

  const uint8_t TEST_MESSAGE = 0;
  Config_add_string(self,
                    "\f\n"
                    " ***** keyman64 version " xstr(VERSION) " ready *****\n"
                    "\n"
                    "    no configuration found in eeprom\n"
                    "      using fallback configuration\n"
                    "\n"
                    "       _v: type firmware version\n"
                    "       _b: enter bootloader\n"
                    "       _f: test crystal frequency\n"
                    "       _t: type this message\n"
                    "\n"
                    "ready.\n");

  const uint8_t FREQUENCY_MESSAGE = 1;
  Config_add_string(self,
                    "\f\n"
                    " *****  keyman64 frequency test  *****\n"
                    "\n"
                    "if the following ten dots are not typed\n"
                    "with about one second delay in between,\n"
                    "check atmel fuses and/or crystal circuit\n");

  const uint8_t READY_MESSAGE = 2;
  Config_add_string(self, "\n\nready.\n");

  binding = Binding_new();
  binding->key = KEY_T;

  command = Command_new();
  command->action = ACTION_TYPE;
  command->mask = TEST_MESSAGE;
  command->data = 0;
  Binding_add(binding, command);

  Config_add_binding(self, binding);

  command = Command_new();
  command->action = ACTION_BOOT;

  binding = Binding_new();
  binding->key = KEY_B;
  Binding_add(binding, command);

  Config_add_binding(self, binding);

  binding = Binding_new();
  binding->key = KEY_F;

  command = Command_new();
  command->action = ACTION_TYPE;
  command->mask = FREQUENCY_MESSAGE;
  command->data = 0;
  Binding_add(binding, command);

  for(int i=0; i<10; i++) {
    command = Command_new();
    command->action = ACTION_KEY_PRESS;
    command->data = 0x25;

    Binding_add(binding, command);

    command = Command_new();
    command->action = ACTION_SLEEP_SHORT;
    command->mask = 0xe8;
    command->data = 0x03;

    Binding_add(binding, command);
  }

  command = Command_new();
  command->action = ACTION_TYPE;
  command->mask = READY_MESSAGE;
  command->data = 0;
  Binding_add(binding, command);

  Config_add_binding(self, binding);

  binding = Binding_new();
  binding->key = KEY_V;

  command = Command_new();
  command->action = ACTION_SHOW_VERSION;
  command->mask = 0;
  command->data = 0;
  Binding_add(binding, command);

  Config_add_binding(self, binding);

  return true;
}

//-----------------------------------------------------------------------------

void Config_reset(volatile Config *self) {
  for(int i=0; i<self->num_bindings; i++) {
    Binding_reset(self->bindings[i]);
  }
}

//-----------------------------------------------------------------------------

void Config_free(Config *self) {
  for(int i=0; i<self->num_bindings; i++) {
    Binding_free(self->bindings[i]);
  }
  free(self->bindings);

  for(int i=0; i<self->num_strings; i++) {
    free(self->strings[i]);
  }
  free(self->strings);
  free(self->longs);
  free(self);
}

//-----------------------------------------------------------------------------

Binding* Binding_new(void) {
  Binding* self = (Binding*) calloc(1, sizeof(Binding));
  self->commands = (Command**) calloc(1, sizeof(Command**));
  self->key = KEY_IMMEDIATE;
  self->state = STATE_EVEN;
  self->times = 1;
  self->count = 1;
  return self;
}

//-----------------------------------------------------------------------------

void Binding_set_key(Binding* self, uint8_t key) {
  self->key = key;
}

//-----------------------------------------------------------------------------

Command* Binding_add(Binding* self, Command* command) {
  self->commands = (Command**) realloc(self->commands, (self->num_commands+1)*sizeof(Command**));
  self->commands[self->num_commands] = command;
  self->num_commands++;
  return command;
}

//-----------------------------------------------------------------------------

void Binding_read(Binding *self, FILE* in) {

  self->times = fgetc(in);

  Command *command;
  uint8_t size = fgetc(in);

  for(int i=0; i<size; i++) {
    command = Binding_add(self, Command_new());
    Command_read(command, in);
  }
}

//-----------------------------------------------------------------------------

void Binding_reset(Binding *self) {
  self->count = self->times;
}

//-----------------------------------------------------------------------------

void Binding_free(Binding *self) {
  for(int i=0; i<self->num_commands; i++) {
    Command_free(self->commands[i]);
  }
  free(self->commands);
  free(self);
}

//-----------------------------------------------------------------------------

Command* Command_new(void) {
  Command* self = (Command*) calloc(1, sizeof(Command));
  self->action = ACTION_NONE;
  self->port = PORT_A;
  self->mask = 0xff;
  self->data = 0xff;
  self->policy = POLICY_ALWAYS;
  return self;
}

//-----------------------------------------------------------------------------

void Command_read(Command *self, FILE* in) {
  self->action = fgetc(in);
  self->policy = fgetc(in);
  self->port   = fgetc(in);
  self->mask   = fgetc(in);
  self->data   = fgetc(in);
}

//-----------------------------------------------------------------------------

void Command_free(Command *self) {
  free(self);
}

//-----------------------------------------------------------------------------

State* State_new(void) {
  State* self = (State*) calloc(1, sizeof(State));
  return self;
}

//-----------------------------------------------------------------------------

void State_free(State* self) {
  free(self);
}

//-----------------------------------------------------------------------------

Expansion* Expansion_new() {
  Expansion* self = (Expansion*) calloc(1, sizeof(Expansion));
  self->num_ports = 0;
  self->ports = NULL;
  self->clock = 0;
  self->data = 0;
  self->latch = 0;
  self->enable = 0;
  return self;
}

//-----------------------------------------------------------------------------

void Expansion_set_num_ports(Expansion* self, uint8_t num_ports) {
  self->num_ports = num_ports;
  self->ports = (uint8_t*) calloc(self->num_ports, sizeof(uint8_t));
}

//-----------------------------------------------------------------------------

void Expansion_read(Expansion* self, FILE* in) {
  Expansion_set_num_ports(self, fgetc(in));
  self->clock  = fgetc(in);
  self->data   = fgetc(in);
  self->latch  = fgetc(in);
  self->enable = fgetc(in);
}

//-----------------------------------------------------------------------------

void Expansion_free(Expansion* self) {
  free(self->ports);
  free(self);
}

//-----------------------------------------------------------------------------

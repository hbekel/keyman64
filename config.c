#include "config.h"

//------------------------------------------------------------------------------

static void ReadConfig(void) {
  uint16_t addr = 0UL;
  uint8_t byte = 0;
  binding_t* binding;

  config = (config_t*) calloc(1, sizeof(config_t));
  config->bindings = (binding_t**) calloc(1, sizeof(binding_t**));
  config->size = 0;
  
  while((byte = ReadEprom(addr)) != 0xffU) {    
    config->bindings = (binding_t**) realloc(config->bindings, (config->size+1)*sizeof(binding_t*));
    binding = (binding_t*) calloc(1, sizeof(binding_t));
    binding->key = (key_t*) calloc(1, sizeof(key_t));
    binding->commands = (command_t**) calloc(1, sizeof(command_t**));    
    ReadBinding(&binding, &addr);
    config->bindings[config->size] = binding;
    config->size++;
  }
}

//------------------------------------------------------------------------------

static void ReadBinding(binding_t** binding, uint16_t* addr) {
  command_t* command;
  ReadKey((*binding)->key, addr);
  (*binding)->size = ReadEprom(*addr); (*(addr))++;
  (*binding)->commands = (command_t**) realloc((*binding)->commands, (*binding)->size*sizeof(command_t*));
  
  for(int i=0; i<(*binding)->size; i++) {
    command = (command_t*) calloc(1, sizeof(command_t));
    ReadCommand(command, addr);
    (*binding)->commands[i] = command;
  }
}

//------------------------------------------------------------------------------

static void ReadKey(key_t* key, uint16_t* addr) {
  uint8_t byte = ReadEprom(*addr); (*(addr))++;
  key->col = (byte & 0xf0) >> 4;
  key->row = (byte & 0x0f);
}

//------------------------------------------------------------------------------

static void ReadCommand(command_t* command, uint16_t* addr) {
  command->action = ReadEprom(*addr); (*(addr))++;
  command->port   = ((command->action & 0x80) == 0) ? 0 : 1;
  command->action &= 0x7f;
  command->mask   = ReadEprom(*addr); (*(addr))++;
  command->data   = ReadEprom(*addr); (*(addr))++;
}

//------------------------------------------------------------------------------

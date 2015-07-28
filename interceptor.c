#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "config.h"

static volatile Config *config;

uint8_t eeprom[46] = { 0xfe, 0x02,
                       0x00, 0xff, 0x00,
                       0x80, 0xff, 0x00,
                       0x70, 0x01, 0x01, 0x40, 0x00,
                       0x73, 0x01, 0x02, 0x0c, 0x00,                       
                       0x10, 0x02, 0x03, 0x0c, 0x00, 0x06, 0x00, 0x20,
                       0x13, 0x01, 0x04, 0x80, 0x00,
                       0x20, 0x03,
                       0x80, 0x01, 0x01,
                       0x05, 0x00, 0xff,
                       0x80, 0x01, 0x00,
                       0xff
};

#include "config.c"

static uint8_t ReadEprom(uint16_t addr) {
  return eeprom[addr];
}

static void WriteConfig(void) {
  for(int i=0; i<sizeof(eeprom); i++) {
    fputc(eeprom[i], stdout);
  }
}

static void PrintConfig(void) {
  for(int i=0; i<config->size; i++) {
    fprintf(stderr, "\ncol = %02X\n", config->bindings[i]->key->col);
    fprintf(stderr, "row = %02X\n", config->bindings[i]->key->row);

    for(int k=0; k<config->bindings[i]->size; k++) {
      fprintf(stderr, "\naction = %02X\n", config->bindings[i]->commands[k]->action);
      fprintf(stderr, "port = %02X\n", config->bindings[i]->commands[k]->port);
      fprintf(stderr, "mask = %02X\n", config->bindings[i]->commands[k]->mask);
      fprintf(stderr, "data = %02X\n", config->bindings[i]->commands[k]->data);
    }
  }
}

int main(int argc, char **argv) {
  ReadConfig();
  PrintConfig();
  WriteConfig();
  return 0;
}

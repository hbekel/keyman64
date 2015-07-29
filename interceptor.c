#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "config.h"
#include "strings.h"
#include "range.h"

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

const char *ws = " \t";

static uint8_t ReadEprom(uint16_t addr) {
  return eeprom[addr];
}

void WriteConfig(void) {
  for(int i=0; i<sizeof(eeprom); i++) {
    fputc(eeprom[i], stdout);
  }
}

void PrintConfig(Config* config) {
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

Key* Key_parse(char* spec) {
  Key* key = NULL;
  uint8_t byte;
  char* invalid;
  
  if(spec[0] == '$') {
    byte = strtol(++spec, &invalid, 16);

    if(spec == invalid) {
      fprintf(stderr, "error: '%s': invalid hex number\n", spec);
    }
    else {
      key = Key_new();
      Key_set(key, byte);
    }
  }
  return key;
}

uint8_t parseAction(char* str) {
  if(str == NULL) return ACTION_NONE;
  
  if(strncasecmp(str, "set",   3) == 0) return ACTION_SET;
  if(strncasecmp(str, "clear", 5) == 0) return ACTION_CLEAR;
  if(strncasecmp(str, "inv",   3) == 0) return ACTION_INVERT;
  if(strncasecmp(str, "inc",   3) == 0) return ACTION_INCREASE;
  if(strncasecmp(str, "dec",   3) == 0) return ACTION_DECREASE;
  if(strncasecmp(str, "tri",   3) == 0) return ACTION_TRISTATE;
  if(strncasecmp(str, "sleep", 5) == 0) return ACTION_SLEEP;
  if(strncasecmp(str, "exec",  4) == 0) return ACTION_EXEC;

  return ACTION_NONE;
}

uint8_t parsePort(char *str) {
  if(str == NULL) return PORT_NONE;

  if(strncasecmp(str, "a",  1) == 0) return PORT_A;
  if(strncasecmp(str, "b",  1) == 0) return PORT_B;

  return PORT_NONE;
}

uint8_t parseBit(char *str) {
  char *invalid;
  uint8_t value = 0;

  if(str == NULL) return 0xff;
  
  value = strtol(str, &invalid, 10);
  if(str == invalid  || value > 7) {
    fprintf(stderr, "error: '%s': not a decimal bit number\n", str);
    return 0xff;
  }
  return 1<<value;
}

uint8_t parseBits(char *str) {

  uint8_t mask = 0;
  Range *bits;
  Range *valid;

  if(str == NULL) return 0xff;

  valid = Range_parse("0-8");
  bits  = Range_parse(str);
  bits->end++;
  
  if(!(Range_valid(bits) || Range_inside(bits, valid))) {
    fprintf(stderr, "error: '%s': invalid bit range\n", str);
    return 0xff;
  }

  for(uint8_t bit=bits->start; bit<bits->end; bit++) {
    mask |= 1<<bit;
  }
  
  return mask;
}

bool parseData(char *str, uint8_t *data) {

  if(str == NULL) return false;
  Key* key;
  uint8_t value;
  char *invalid;

  if((key = Key_parse(str)) != NULL) {
    *data = Key_get(key);
    free(key);
    return true;
  }

  value = strtol(str, &invalid, 10);
  
  if(str == invalid) {
    return false;
  }
  *data = value;
  return true;
}

bool equal(const char *a, const char *b) {
  if(a == NULL || b == NULL) return false;
  return strcasecmp(a, b) == 0;
}

Command* Command_parse(char* spec) {
  Command* command = Command_new();
  StringList* words = StringList_new();
  uint8_t data;
  bool error = false;
  int i = 0;
  
  StringList_append_tokenized(words, spec, ws);

  if((command->action = parseAction(StringList_get(words, i++))) == ACTION_NONE) {
    fprintf(stderr, "error: '%s': invalid command\n", StringList_get(words, i-1));
    goto error;
  }
  
  if(equal(StringList_get(words, i), "port")) {
    if((command->port = parsePort(StringList_get(words, ++i))) == PORT_NONE) {
      fprintf(stderr, "error: '%s': invalid port\n", StringList_get(words, i-1));
      goto error;
    }
    i++;
  }
  
  if(equal(StringList_get(words, i), "bit")) {
    if((command->mask = parseBit(StringList_get(words, ++i))) == 0xff) {
      fprintf(stderr, "error: '%s': invalid bit\n", StringList_get(words, i-1));
      goto error;
    }
    i++;
  }

  if(equal(StringList_get(words, i), "bits")) {
    if((command->mask = parseBits(StringList_get(words, ++i))) == 0xff) {
      fprintf(stderr, "error: '%s': invalid bit range\n", StringList_get(words, i-1));
      goto error;
    }
    i++;
  }  

  if(equal(StringList_get(words, i), "to") || equal(StringList_get(words, i), "=")) {
    i++;
  }  

  if(i < words->size) {
    if(!parseData(StringList_get(words, i), &data)) {
      fprintf(stderr, "error: '%s': invalid value\n", StringList_get(words, i));
      goto error;
    }
    command->data = data;
  }
  
 done:
  StringList_free(words);
  return command;

 error:
  free(command);
  command = NULL;
  goto done;
}

bool ParseConfig(char* filename, Config* config) {

  FILE* file;
  char* buffer = (char*) calloc(4096, sizeof(char));

  char* line;
  char* colon;
  char *keyspec;
  int pos = 0;
  
  Binding* binding;
  Key* key;
  Command* command;
    
  if((file = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "%s: %s\n", filename, strerror(errno));
    return false;      
  }

  while(fgets(buffer, 4096, file) != NULL) {
    line = buffer;
    pos++;
    
    // skip leading whitespace
    line += strspn(line, ws);

    // skip empty lines and comments
    if(line[0] == '\n' || line[0] == '#') continue;

    // discard newline
    line[strlen(line)-1] = '\0';

    // check if this command shall be bound to a key
    if((colon = strstr(line, ":")) != NULL) {
      keyspec = line;
      colon[0] = '\0';
      line = colon+1;

      // try to parse the keyspec...
      if((key = Key_parse(keyspec)) == NULL) {
	fprintf(stderr, "error: line %d: '%s': invalid key specification\n", pos, keyspec);
	return false;
      }
    }
    else {
      // no keyspec given -> bind to initial "key"
      key = Key_clone(&KEY_INIT);
    }

    // skip leading whitespace
    line += strspn(line, ws);
    
    // try to parse command...
    if((command = Command_parse(line)) == NULL) {
      fprintf(stderr, "error: line %d: '%s': invalid command specification\n", pos, line);
      return false;
    }

    // get existing binding or add new binding...
    if(!Config_has_binding(config, key)) {
      binding = Config_get_binding(config, key);
    }
    else {
      binding = Binding_new();
      Binding_set_key(binding, key);
      Config_add(config, binding);
    }

    // append command to binding
    Binding_add(binding, command);
    free(key);   
  }

  free(buffer);
  fclose(file);
  return true;
}

int main(int argc, char **argv) {
  argc--;
  argv++;

  Config* config = Config_new();
  
  if(argc == 1) {
    if(ParseConfig(argv[0], config)) {
      PrintConfig(config);
    }
  }    
  return 0;
}

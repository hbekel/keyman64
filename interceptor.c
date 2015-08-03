#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "strings.h"
#include "range.h"
#include "symbols.h"
#include "interceptor.h"

#include "config.c"

//------------------------------------------------------------------------------

typedef enum { BINARY, CONFIG } Format;
const char *ws = " \t";

//------------------------------------------------------------------------------
// Utility functions for parsing
//------------------------------------------------------------------------------

static uint8_t parseAction(char* str) {
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

//------------------------------------------------------------------------------

static uint8_t parsePort(char *str) {
  if(str == NULL) return PORT_NONE;

  if(strncasecmp(str, "a",  1) == 0) return PORT_A;
  if(strncasecmp(str, "b",  1) == 0) return PORT_B;

  return PORT_NONE;
}

//------------------------------------------------------------------------------

static uint8_t parseBit(char *str) {
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

//------------------------------------------------------------------------------

static uint8_t parseBits(char *str, bool *error) {

  uint8_t mask = 0;
  Range *bits;
  Range *valid;

  if(str == NULL) return 0xff;

  valid = Range_parse("0-7");
  bits  = Range_parse(str);

  if(!(Range_valid(bits) && Range_inside(bits, valid))) {
    fprintf(stderr, "error: '%s': invalid bit range\n", str);
    *error = true;
    return 0xff;
  }

  for(uint8_t bit=bits->start; bit<bits->end+1; bit++) {
    mask |= 1<<bit;
  }
  
  return mask;
}

//------------------------------------------------------------------------------

static bool parseData(char *str, uint8_t *data) {

  if(str == NULL) return false;
  
  bool result = true;

  uint8_t value;
  char *invalid;
  int base = 10;

  Key* key = Key_new();
  
  if(Key_parse(key, str, false)) {
    *data = Key_get(key);
    goto done;
  }
  
  if(str[0] == '%') {
    base = 2; str++;
  }
  value = strtol(str, &invalid, base);
  
  if(str == invalid) {
    result = false;
    goto done;
  }
  
  *data = value;
  
 done:
  free(key);
  return result;    
}

//------------------------------------------------------------------------------

static bool equal(const char *a, const char *b) {
  if(a == NULL || b == NULL) return false;
  return strcasecmp(a, b) == 0;
}

//------------------------------------------------------------------------------
// Functions for parsing config files
//------------------------------------------------------------------------------

bool Config_parse(Config* self, FILE* in) {

  char* buffer = (char*) calloc(4096, sizeof(char));
  char* line;
  char* colon;
  char *keyspec;
  int pos = 0;
  
  Binding* binding;
  Command* command;
  Key* key;

  fseek(in, 0, SEEK_SET);
    
  while(fgets(buffer, 4096, in) != NULL) {
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
      key = Key_new();
      
      if(!Key_parse(key, keyspec, false)) {
        fprintf(stderr, "error: line %d: '%s': invalid key specification\n", pos, keyspec);
	free(key);
        return false;
      }
    }
    else {
      // no keyspec given -> bind to initial "key"
      key = Key_new();
      Key_set(key, Key_get(&KEY_INIT));
    }

    // skip leading whitespace of command spec
    line += strspn(line, ws);

    // try to parse command...        
    command = Command_new();

    if(!Command_parse(command, line)) {
      fprintf(stderr, "error: line %d: '%s': invalid command specification\n", pos, line);
      free(command);
      return false;
    }

    // get existing binding or add new binding...
    if(!Config_has_binding(self, key)) {
      binding = Config_get_binding(self, key);
    }
    else {
      binding = Binding_new();
      Binding_set_key(binding, key);
      Config_add(self, binding);
    }

    // append command to binding
    Binding_add(binding, command);
    free(key);   
  }

  free(buffer);
  return true;
}

//------------------------------------------------------------------------------

bool Key_parse(Key *self, char* spec, bool reportUnknownSymbol) {

  bool result = false;
  uint8_t byte;
  char* invalid;

  if(spec[0] == '$') {
    byte = strtol(++spec, &invalid, 16);

    if(spec == invalid) {
      fprintf(stderr, "error: '%s': invalid hex number\n", spec);
      goto done;
    }
    else {
      Key_set(self, byte);
      result = true;
      goto done;
    }
  }

  for(int i=0; i<sizeof(symbols)/sizeof(Symbol); i++) {
    if(strcasecmp(symbols[i].name, spec) == 0) {
      Key_set(self, Key_get(&(symbols[i].key)));
      result = true;
      goto done;
    }
  }

  if(!result && reportUnknownSymbol) {
    fprintf(stderr, "error: '%s': unknown key name\n", spec);
  }
  
 done:
  return result;
}

//------------------------------------------------------------------------------

bool Command_parse(Command* self, char* spec) {

  bool result = true;
  bool error = false;
  StringList* words = StringList_new();
  uint8_t data;
  int i = 0;
  
  StringList_append_tokenized(words, spec, ws);

  if((self->action = parseAction(StringList_get(words, i++))) == ACTION_NONE) {
    fprintf(stderr, "error: '%s': invalid command\n", StringList_get(words, i-1));
    goto error;
  }
  
  if(equal(StringList_get(words, i), "port")) {
    if((self->port = parsePort(StringList_get(words, ++i))) == PORT_NONE) {
      fprintf(stderr, "error: '%s': invalid port\n", StringList_get(words, i));
      goto error;
    }
    i++;
  }
  
  if(equal(StringList_get(words, i), "bit")) {
    if((self->mask = parseBit(StringList_get(words, ++i))) == 0xff) {
      fprintf(stderr, "error: '%s': invalid bit\n", StringList_get(words, i));
      goto error;
    }
    i++;
  }

  if(equal(StringList_get(words, i), "bits")) {

    self->mask = parseBits(StringList_get(words, ++i), &error);

    if(error) {
      fprintf(stderr, "error: '%s': invalid bit range\n", StringList_get(words, i));
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
    self->data = data;
  }
  
 done:
  StringList_free(words);
  return result;

 error:
  result = false;
  goto done;
}

//------------------------------------------------------------------------------
// Functions for writing binary configuration
//------------------------------------------------------------------------------

void Config_write(Config *self, FILE *out) {
  fputc(CONFIG_MAGIC[0], out);
  fputc(CONFIG_MAGIC[1], out);
  
  for(int i=0; i<self->size; i++) {
    Binding_write(self->bindings[i], out);
  }
  fputc(0xffU, out);
}

//------------------------------------------------------------------------------

void Binding_write(Binding *self, FILE* out) {
  Key_write(self->key, out);
  fputc(self->size, out);
  
  for(int i=0; i<self->size; i++) {
    Command_write(self->commands[i], out);
  }
}

//------------------------------------------------------------------------------

void Key_write(Key *self, FILE* out) {
  fputc(Key_get(self), out);
}

//------------------------------------------------------------------------------

void Command_write(Command *self, FILE* out) {
  uint8_t action = self->action;
  action |= self->port<<7;
  fputc(action, out);
  fputc(self->mask, out);
  fputc(self->data, out);
}

//------------------------------------------------------------------------------
// Functions for writing canonical config file format
//------------------------------------------------------------------------------

void Config_print(Config *self, FILE* out) {
  for(int i=0; i<self->size; i++) {
    Binding_print(self->bindings[i], out);
  }
}

//------------------------------------------------------------------------------

void Binding_print(Binding *self, FILE* out) {
  
  for(int i=0; i<self->size; i++) {
    Key_print(self->key, out);
    Command_print(self->commands[i], out);
  }
}

//------------------------------------------------------------------------------

void Key_print(Key *self, FILE* out) {
  fprintf(out, "$%02X: ", Key_get(self));	 
}

//------------------------------------------------------------------------------

void Command_print(Command *self, FILE* out) {

  char* action = "unknown";
  
  switch(self->action) {
  case ACTION_SET:      action = "set";      break;
  case ACTION_CLEAR:    action = "clear";    break;
  case ACTION_INVERT:   action = "invert";   break;
  case ACTION_INCREASE: action = "increase"; break;
  case ACTION_DECREASE: action = "decrease"; break;
  case ACTION_TRISTATE: action = "tristate"; break;
  case ACTION_SLEEP:    action = "sleep";    break;
  case ACTION_EXEC:     action = "exec";     break;
  };

  uint8_t mask  = 0;
  uint8_t start = 0;
  uint8_t end   = 0;
  
  if(self->mask) {
    mask = self->mask;
    while((mask & 1) == 0) {
      mask = mask >> 1;
      start++;
    }

    end = 7;
    mask = self->mask;
    while((mask & 0x80) == 0) {
      mask = mask << 1;
      end--;
    }
  }
  
  fprintf(out, "%s port %s bits %d-%d $%02X\n",
	  action,
	  (self->port == PORT_A) ? "a" : "b",
          start,
	  end,
	  self->data);
	  
}

//------------------------------------------------------------------------------
// Usage: interceptor [<infile=->] [<outfile=->]
//------------------------------------------------------------------------------

int main(int argc, char **argv) {

  int result = EXIT_SUCCESS;
  
  FILE *in  = stdin;
  FILE *out = stdout;
  Format output_format = BINARY;

  Config* config = Config_new();

  argc--; argv++;
  
  if(argc >= 1 && (strncmp(argv[0], "-", 1) != 0)) {

    if((in = fopen(argv[0], "rb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
      return EXIT_FAILURE;      
    }
  }

  if(argc >= 2 && (strncmp(argv[1], "-", 1) != 0)) {

    if((out = fopen(argv[1], "wb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
      return EXIT_FAILURE;      
    }
    if(strncasecmp(argv[1]+(strlen(argv[1])-5), ".conf", 5) == 0) {
      output_format = CONFIG;
    }
  }

  if(Config_read(config, in) || Config_parse(config, in)) {

    output_format == BINARY ?
      Config_write(config, out) :
      Config_print(config, out);
  }
  else {
    result = EXIT_FAILURE;
  }

  fclose(in);
  fclose(out);

  return result;
}

//------------------------------------------------------------------------------

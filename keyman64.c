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
#include "keyman64.h"

#include "config.c"

//------------------------------------------------------------------------------

Config* config;

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
  if(strncasecmp(str, "meta",  4) == 0) return ACTION_DEFINE_META;
  if(strncasecmp(str, "down",  2) == 0) return ACTION_KEY_DOWN;
  if(strncasecmp(str, "up",    2) == 0) return ACTION_KEY_UP;
  if(strncasecmp(str, "type",  4) == 0) return ACTION_TYPE;  
  if(strncasecmp(str, "boot",  4) == 0) return ACTION_BOOT;    
  if(strncasecmp(str, "swap",  4) == 0) return ACTION_SWAP;  
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

  uint8_t key;
  
  if(Key_parse(&key, str, false)) {
    *data = key;
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
  char *comment;
  int pos = 0;
  
  Binding* binding;
  Command* command;
  uint8_t key;

  fseek(in, 0, SEEK_SET);
    
  while(fgets(buffer, 4096, in) != NULL) {
    line = buffer;
    pos++;
    
    // skip leading whitespace
    line += strspn(line, ws);

    // skip empty lines and comments
    if(line[0] == '\n' || line[0] == '#') continue;

    // remove comment at the end of the line
    if((comment = strstr(line, "#")) != NULL) {
      comment[0] = '\0';
    }

    // discard newline
    if(line[strlen(line)-1] == '\n') {
      line[strlen(line)-1] = '\0';
    }

    // check if this command shall be bound to a key
    if((colon = strstr(line, ":")) != NULL) {
      keyspec = line;
      colon[0] = '\0';
      line = colon+1;

      // try to parse the keyspec...
      if(!Key_parse(&key, keyspec, false)) {
        fprintf(stderr, "error: line %d: '%s': invalid key specification\n", pos, keyspec);
        return false;
      }
    }
    else {
      // no keyspec given -> bind to initial "key"
      key = KEY_INIT;
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
      Config_add_binding(self, binding);
    }

    // append command to binding
    Binding_add(binding, command);
  }

  free(buffer);
  return true;
}

//------------------------------------------------------------------------------

bool Key_parse(uint8_t *key, char* spec, bool reportUnknownSymbol) {

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
      *key = byte;
      result = true;
      goto done;
    }
  }

  for(int i=0; i<sizeof(symbols)/sizeof(Symbol); i++) {
    if(strcasecmp(symbols[i].name, spec) == 0) {
      *key = symbols[i].key;
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
  char* str;
  uint8_t data;
  uint16_t index;
  uint16_t value;
  int i = 0;
  
  StringList_append_tokenized(words, spec, ws);

  if((self->action = parseAction(StringList_get(words, i++))) == ACTION_NONE) {
    fprintf(stderr, "error: '%s': invalid command\n", StringList_get(words, i-1));
    goto error;
  }

  if(self->action == ACTION_TYPE) {
    str = spec + strcspn(spec, ws) + 1;
    index = Config_add_string(config, str);
    self->mask = index & 0xff;
    self->data = (index >> 8) & 0xff;
    goto done;
  }

  if(self->action == ACTION_SLEEP) {
    str = StringList_get(words, i);

    if(str == NULL) {
      fprintf(stderr, "error: missing duration for sleep\n");
      goto error;
    }

    char *invalid;
    value = strtol(str, &invalid, 10);

    if(str == invalid) {
      fprintf(stderr, "error: '%s': not a number\n", str);
      goto error;
    }
    
    self->mask = value & 0xff;
    self->data = (value >> 8) & 0xff;    
  }

  if(self->action == ACTION_SWAP) {
    if(!parseData(StringList_get(words, i), &data)) {
      if(StringList_get(words, i) != NULL) {
        fprintf(stderr, "error: '%s': invalid key spec\n", StringList_get(words, i));
      }
      else {
        fprintf(stderr, "error: swap: missing key spec\n");
        goto error;
      }
    }
    self->mask = data;

    i++;    
    if(!parseData(StringList_get(words, i), &data)) {
      if(StringList_get(words, i) != NULL) {
        fprintf(stderr, "error: '%s': invalid key spec\n", StringList_get(words, i));
      }
      else {
        fprintf(stderr, "error: swap: missing key spec\n");
        goto error;
      }
    }
    self->data = data;
    
    goto done;
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
  for(int i=0; i<self->_size; i++) {
    fputc(KEY_STRING, out);
    fputs(self->strings[i], out);
    fputc('\0', out);
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

void Key_write(uint8_t key, FILE* out) {
  fputc(key, out);
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
    if(self->key != KEY_INIT) {
      Key_print(self->key, out);
    }
    Command_print(self->commands[i], out);
  }
}

//------------------------------------------------------------------------------

void Key_print(uint8_t key, FILE* out) {
  fprintf(out, "$%02X: ", key);	 
}

//------------------------------------------------------------------------------

void Command_print(Command *self, FILE* out) {

  char* action = "unknown";
  
  switch(self->action) {
  case ACTION_SET:         action = "set";      break;
  case ACTION_CLEAR:       action = "clear";    break;
  case ACTION_INVERT:      action = "invert";   break;
  case ACTION_INCREASE:    action = "increase"; break;
  case ACTION_DECREASE:    action = "decrease"; break;
  case ACTION_TRISTATE:    action = "tristate"; break;
  case ACTION_SLEEP:       action = "sleep";    break;
  case ACTION_EXEC:        action = "exec";     break;
  case ACTION_DEFINE_META: action = "meta";     break;
  case ACTION_KEY_DOWN:    action = "down";     break;
  case ACTION_KEY_UP:      action = "up";       break;
  case ACTION_TYPE:        action = "type";     break;
  case ACTION_BOOT:        action = "boot";     break;                
  case ACTION_SWAP:        action = "swap";     break;                
  };

  if(self->action == ACTION_TYPE) {
    fprintf(out, "%s ", action);
    fprintf(out, "%s\n", config->strings[self->data]);
    return;
  }
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

  if(self->action == ACTION_BOOT) {
    fprintf(out, "%s", action);
  }

  else if(self->action == ACTION_SLEEP) {
    uint16_t value = self->mask | (self->data << 8);
    fprintf(out, "%s %d", action, value);
  }
  
  else if(self->action == ACTION_DEFINE_META ||
	  self->action == ACTION_EXEC ||
	  self->action == ACTION_KEY_DOWN ||
	  self->action == ACTION_KEY_UP) {

    fprintf(out, "%s $%02X", action, self->data);
    
  }
  else if(self->action == ACTION_SWAP) {
    fprintf(out, "%s $%02X $%02X", action, self->mask, self->data);
  }
  else {
    if(start == end) {

      fprintf(out, "%s port %s bit %d ",
	      action,
	      (self->port == PORT_A) ? "a" : "b",
	      start);
    } else {
      
      fprintf(out, "%s port %s bits %d-%d ",
	      action,
	      (self->port == PORT_A) ? "a" : "b",
	      start,
	      end);
    }
  }
  if(self->action == ACTION_SET) {
    fprintf(out, "$%02X", self->data);
  }
  fprintf(out, "\n");
}

//------------------------------------------------------------------------------
// Usage: keyman64 [<infile=->] [<outfile=->]
//------------------------------------------------------------------------------

int main(int argc, char **argv) {

  int result = EXIT_SUCCESS;
  
  FILE *in  = stdin;
  FILE *out = stdout;
  Format output_format = BINARY;

  config = Config_new();

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

  if(in == stdin) {
    fprintf(stderr, "reading from stdin...\n");
  }

  if(out == stdout) {
    fprintf(stderr, "writing to stdout...\n");
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

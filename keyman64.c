#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#if windows
  #include <windows.h>
#endif

#include "config.h"
#include "strings.h"
#include "range.h"
#include "symbols.h"
#include "keyman64.h"
#include "usb.h"
#include "protocol.h"
#include "target.h"

#include "config.c"

//------------------------------------------------------------------------------

Config* config = NULL;

typedef enum { BINARY, CONFIG } Format;
const char *ws = " \t";
char *device;
uint16_t delay = 0;

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
  if(strncasecmp(str, "sleep", 5) == 0) return ACTION_SLEEP_SHORT;
  if(strncasecmp(str, "exec",  4) == 0) return ACTION_EXEC;
  if(strncasecmp(str, "meta",  4) == 0) return ACTION_DEFINE_META;
  if(strncasecmp(str, "down",  2) == 0) return ACTION_KEY_DOWN;
  if(strncasecmp(str, "up",    2) == 0) return ACTION_KEY_UP;
  if(strncasecmp(str, "type",  4) == 0) return ACTION_TYPE;  
  if(strncasecmp(str, "boot",  4) == 0) return ACTION_BOOT;    
  if(strncasecmp(str, "swap",  4) == 0) return ACTION_SWAP;  
  if(strncasecmp(str, "press", 5) == 0) return ACTION_KEY_PRESS;
  if(strncasecmp(str, "using", 5) == 0) return ACTION_DEFINE_SWITCH;    
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
    mask = 0xff;
    goto done;
  }

  for(uint8_t bit=bits->start; bit<bits->end+1; bit++) {
    mask |= 1<<bit;
  }
  
 done:
  Range_free(valid);
  Range_free(bits);
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

static uint32_t parseDuration(char *str) {

  uint64_t total = 0;;
  uint8_t days = 0;
  uint16_t hours = 0;
  uint32_t minutes = 0;
  uint32_t seconds = 0;
  uint32_t milliseconds = 0;
  uint32_t n = 0;

  // d, h, m, s, ms
  
  char *next;
  
  while(true) {
    n = strtol(str, &next, 10);

    if(str == next) {
      break;
    }
    
    else if(strncmp(next, "ms", 2) == 0) {
      milliseconds = n;
      str = next + 2;
    }

    else if(strncmp(next, "s", 1) == 0) {
      seconds = n;
      str = next + 1;
    }

    else if(strncmp(next, "m", 1) == 0) {
      minutes = n;
      str = next + 1;
    }

    else if(strncmp(next, "h", 1) == 0) {
      hours = n;
      str = next + 1;
    }

    else if(strncmp(next, "d", 1) == 0) {
      days = n;
      str = next + 1;
    }
    else if(strlen(next) == 0) {
      milliseconds += n;
      break;
    }
  }
  total += milliseconds;
  total += seconds*1000;
  total += minutes*60*1000;
  total += hours*60*60*1000;
  total += days*24*60*60*1000;

  return (uint32_t) total;
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
    if(line[0] == '\n' || line[0] == '\r' || line[0] == '#') continue;

    // remove comment at the end of the line
    if((comment = strstr(line, "#")) != NULL) {
      comment[0] = '\0';
    }

    // discard newlines
    if(line[strlen(line)-1] == '\n') {
      line[strlen(line)-1] = '\0';
    }

    if(line[strlen(line)-1] == '\r') {
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
      // no keyspec given -> bind to immediate "key"
      key = KEY_IMMEDIATE;
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
  int base = 0;

  if(spec[0] == '$') {
    spec++;
    base = 16;
  }
  
  byte = strtol(spec, &invalid, base);

  if(spec != invalid) {
    *key = byte;
    result = true;
    goto done;
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
  uint32_t value;
  int i = 0;
  
  StringList_append_tokenized(words, spec, ws);

  if((self->action = parseAction(StringList_get(words, i++))) == ACTION_NONE) {
    fprintf(stderr, "error: '%s': invalid command\n", StringList_get(words, i-1));
    goto error;
  }

  if(self->action == ACTION_TYPE) {
    str = spec + strcspn(spec, ws) + 1;

    if(!Config_has_string(config, str, &index)) {
      index = Config_add_string(config, str);
    }

    self->mask = index & 0xff;
    self->data = (index >> 8) & 0xff;
    goto done;
  }

  if(self->action == ACTION_SLEEP_SHORT) {
    str = StringList_get(words, i);

    if(str == NULL) {
      fprintf(stderr, "error: missing duration for sleep\n");
      goto error;
    }

    value = parseDuration(str);

    if(value <= 65535) {
      self->mask = value & 0xff;
      self->data = (value >> 8) & 0xff;
    }
    else {
      self->action = ACTION_SLEEP_LONG;
      if(!Config_has_long(config, value, &index)) {
        index = Config_add_long(config, value);
      }
      self->mask = index & 0xff;
      self->data = (index >> 8) & 0xff;
    }
    goto done;
  }

  if(self->action == ACTION_DEFINE_SWITCH) {
    str = StringList_get(words, i);

    if(str == NULL) {
      fprintf(stderr, "error: missing crosspoint switch model for 'using'\n");
      goto error;
    }

    if(strstr(str, "22106") != NULL) {
      self->data = SWITCH_22106;
      goto done;
    }
    
    if(strstr(str, "8808") != NULL) {
      self->data = SWITCH_8808;
      goto done;
    }

    fprintf(stderr, "error: please specify crosspoint switch model (CD74HC22106 or MT8808)\n");
    goto error;    
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
  for(int i=0; i<self->__size; i++) {
    fputc(KEY_LONG, out);
    fputc((self->longs[i] >> 0) & 0xff, out);
    fputc((self->longs[i] >> 8) & 0xff, out);
    fputc((self->longs[i] >> 16) & 0xff, out);
    fputc((self->longs[i] >> 24) & 0xff, out);
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
    if(self->key != KEY_IMMEDIATE) {
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
  case ACTION_SET:           action = "set";      break;
  case ACTION_CLEAR:         action = "clear";    break;
  case ACTION_INVERT:        action = "invert";   break;
  case ACTION_INCREASE:      action = "increase"; break;
  case ACTION_DECREASE:      action = "decrease"; break;
  case ACTION_TRISTATE:      action = "tristate"; break;
  case ACTION_SLEEP_SHORT:   action = "sleep";    break;
  case ACTION_SLEEP_LONG:    action = "sleep";    break;    
  case ACTION_EXEC:          action = "exec";     break;
  case ACTION_DEFINE_META:   action = "meta";     break;
  case ACTION_KEY_DOWN:      action = "down";     break;
  case ACTION_KEY_UP:        action = "up";       break;
  case ACTION_TYPE:          action = "type";     break;
  case ACTION_BOOT:          action = "boot";     break;                
  case ACTION_SWAP:          action = "swap";     break;
  case ACTION_KEY_PRESS:     action = "press";    break;
  case ACTION_DEFINE_SWITCH: action = "using";    break;                        
  };

  if(self->action == ACTION_TYPE) {
    uint16_t index = self->mask | (self->data << 8);
    fprintf(out, "%s ", action);
    fprintf(out, "%s\n", config->strings[index]);
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

  else if(self->action == ACTION_SLEEP_SHORT) {
    uint16_t short_value = self->mask | (self->data << 8);
    fprintf(out, "%s %d", action, short_value);
  }

  else if(self->action == ACTION_SLEEP_LONG) {
    uint16_t index = self->mask | (self->data << 8);
    uint32_t long_value = config->longs[index];
    fprintf(out, "%s %d", action, long_value);
  }

  else if(self->action == ACTION_DEFINE_SWITCH) {
    fprintf(out, "%s %s", action,
            self->data == SWITCH_22106 ? "CD74HC22106" : "MT8808");
  }
  
  else if(self->action == ACTION_DEFINE_META ||
	  self->action == ACTION_EXEC ||
	  self->action == ACTION_KEY_DOWN ||
	  self->action == ACTION_KEY_UP ||
	  self->action == ACTION_KEY_PRESS) {

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

#if defined(WIN32) && !defined(__CYGWIN__)
FILE* fmemopen(void *__restrict buf, size_t size, const char *__restrict mode) {

  FILE* result;
  char path[MAX_PATH+1]; 
  char file[MAX_PATH+1];

  if(!GetTempPath(MAX_PATH+1, path)) return NULL;
  if(!GetTempFileName(path, "key", 0, file)) return NULL;

  result = fopen(file, "wbD+");
  fwrite(buf, sizeof(char), size, result);
  fseek(result, 0, SEEK_SET);
  
  return result;
}
#endif

void fmemupdate(FILE *fp, void *buf,  uint16_t size) {
#if defined(WIN32) && !defined(__CYGWIN__)
  fseek(fp, 0, SEEK_SET);
  fread(buf, sizeof(uint8_t), size, fp);
#endif
}

//------------------------------------------------------------------------------

int main(int argc, char **argv) {

#if linux
  const char* default_device = "/dev/keyman64";
#else
  const char* default_device = "usb";
#endif

  int result = EXIT_SUCCESS;
  
  device = (char*) calloc(strlen(default_device)+1, sizeof(char));
  strcpy(device, default_device);
  
  struct option options[] = {
    { "help",    no_argument,       0, 'h' },
    { "version", no_argument,       0, 'v' },
    { "device",  required_argument, 0, 'd' },
    { "keys",    no_argument,       0, 'k' },
    { "delay",   required_argument, 0, 'D' },
    { 0, 0, 0, 0 },
  };
  int option, option_index;


  while(1) {
    option = getopt_long(argc, argv, "hvd:kD:", options, &option_index);

    if(option == -1)
      break;
    
    switch (option) {
      
    case 'h':
      usage();
      goto done;
      break;      
      
    case 'v':
      version();
      goto done;
      break;
      
    case 'd':
      device = (char*) realloc(device, strlen(optarg)+1);
      strcpy(device, optarg);
      break;
      
    case 'k':
      keys();
      goto done;
      break;

    case 'D':
      delay = strtol(optarg, NULL, 0);
      break;
    }
  }

  argc -= optind;
  argv += optind;

  if(argc && (strcmp(argv[0], "convert") == 0)) {
    result = convert(--argc, ++argv);
  }
  else {
    result = command(argc, argv);
  }
  
 done:
  free(device);
  return result;
}

//------------------------------------------------------------------------------

static void join(char** dst, char **src, int size) {

  int required = 0;
  for(int i=0; i<size; i++) {
    required += strlen(src[i])+1;
  }
  required += 1;

  *dst = realloc(*dst, required*sizeof(char));
  
  for(int i=0; i<size; i++) {
    strcat(*dst, src[i]);
    if(i<size-1) {
      strcat(*dst, " ");
    }
  }
}

//------------------------------------------------------------------------------

int command(int argc, char **argv) {
  int result = EXIT_FAILURE;
  
  FILE *in = stdin;
  FILE *out = NULL;
  char *str = (char*) calloc(1, sizeof(char));
  uint8_t *data = NULL;
  uint16_t size = 0;
  
  libusb_device_handle *handle = NULL;
  DeviceInfo info;
  
  if((result = libusb_init(NULL)) < 0) {
    fprintf(stderr, "error: could not initialize libusb-1.0: %s\n", libusb_strerror(result));
    goto done;
  }

  if(argc) {

    if((in = fopen(argv[0], "rb")) == NULL) {
    
      join(&str, argv, argc);
    
      if(strlen(str)) {
        if((in = fmemopen(str, strlen(str)+1, "rb")) == NULL) {
          fprintf(stderr, "error: could not open string via fmemopen(): %s \n", strerror(errno));
          goto done;
        }
      }
    }
  }

  if(in == stdin) {
    fprintf(stderr, "reading commands from stdin... (keyman64 --help for instructions)\n");
  }
  
  config = Config_new();

  if(!(Config_read(config, in) || Config_parse(config, in))) {
    goto done;
  }
  fclose(in);
  
  data = (uint8_t*) calloc(4096, sizeof(char));
  
  if((out = fmemopen(data, 4096, "wb")) == NULL) {
    fprintf(stderr, "error: %s\n", strerror(errno));
    goto done;
  }
  
  Config_write(config, out);
  size = ftell(out);
  fmemupdate(out, data, size);  
  fclose(out);

  info.vid = KEYMAN64_VID;
  info.pid = KEYMAN64_PID;

  usb_lookup(device, &info);
  handle = usb_open(NULL, &info);
  
  if(handle == NULL) {
    fprintf(stderr, "error: could not open %s\n", device);
    goto done;
  }

  if((result = usb_send(handle, KEYMAN64_CTRL, delay, data, size)) < 0) {
    fprintf(stderr, "error: could send usb control message: %s\n", libusb_strerror(result));
    goto done;
  }
  result = EXIT_SUCCESS;
  
 done:
  if(handle != NULL) {
    libusb_close(handle);
  }
  libusb_exit(NULL);
  
  if(config != NULL) {
    Config_free(config);
  }

  if(data != NULL) {
    free(data);
  }
  free(str);
  return result;
}

//------------------------------------------------------------------------------

int convert(int argc, char **argv) {

  int result = EXIT_FAILURE;

  FILE *in  = stdin;
  FILE *out = stdout;
  Format output_format = BINARY;

  config = Config_new();
  
  if(argc >= 1 && (strncmp(argv[0], "-", 1) != 0)) {

    if((in = fopen(argv[0], "rb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
      goto done;
    }
  }

  if(argc >= 2 && (strncmp(argv[1], "-", 1) != 0)) {

    if((out = fopen(argv[1], "wb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
      goto done;
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

    result = EXIT_SUCCESS;
  }

 done:
  Config_free(config);
  fclose(in);
  fclose(out);

  return result;
}

//------------------------------------------------------------------------------

void version(void) {
  printf("keyman64 version %.1f (c) 2015 Henning Bekel. All rights reserverd.\n", VERSION);
}

//------------------------------------------------------------------------------

void usage(void) {
  version();
  printf("\n");
  printf("Usage:\n");
  printf("      keyman64 <options>\n");
  printf("      keyman64 convert [<infile>|-] [<outfile>|-]\n");
  printf("      keyman64 <command..>\n");
  printf("      keyman64 <file>\n");    
  printf("\n");
  printf("  Options:\n");
  printf("           -v, --version : print version information\n");
  printf("           -h, --help    : print this help text\n");
#if linux
  printf("           -d, --device  : specify usb device (default: /dev/keyman64\n");
#elif windows
  printf("           -d, --device  : specify usb device (default: usb\n");
#endif
  printf("           -D, --delay   : delay in ms between commands\n");  
  printf("           -k, --keys    : list key names and synonyms\n");
  printf("\n");
  printf("  Conversion arguments:\n");
  printf("           <infile>  : input file, format is autodetected\n");
  printf("           <outfile> : output file, format determined by extension:\n");
  printf("\n");
  printf("           *.conf : plain text config file format\n");
  printf("           *.bin  : binary file format (default)\n");  
  printf("\n");
  printf("           Missing arguments default to stdin or stdout\n");
  printf("\n");
  printf("  Commands:\n");
  printf("            (any configuration command)\n");
  printf("\n");  
}

//------------------------------------------------------------------------------

void keys(void) {
  Symbol symbol;
  Symbol synonym;
  int total = sizeof(symbols)/sizeof(Symbol);

  for(int i=0; i<64; i++) {
    printf("%02X: ", i);
    symbol = symbols[i];
    printf("%s", symbol.name);
    for(int k=64; k<total; k++) {
      synonym = symbols[k];
      if(symbol.key == synonym.key) {
        printf(", %s", synonym.name);
      }
    }
    printf("\n");
  }
}

//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
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
#include "usb.h"
#include "keyman64.h"
#include "protocol.h"
#include "target.h"
#include "intelhex.h"
#include "config.c"

//-----------------------------------------------------------------------------

Config* config = NULL;
bool ignoreMissingPorts = false;

typedef enum { BINARY, CONFIG } Format;
const char *ws = " \t";
uint16_t delay = 0;

char *device;
DeviceInfo keyman64;
DeviceInfo usbasp;

//-----------------------------------------------------------------------------
// Utility functions for parsing
//-----------------------------------------------------------------------------

static uint8_t parseAction(char* str) {
  if(str == NULL) return ACTION_NONE;

  if(strcasecmp(str, "set"      ) == 0) return ACTION_SET;
  if(strcasecmp(str, "clear"    ) == 0) return ACTION_CLEAR;
  if(strcasecmp(str, "invert"   ) == 0) return ACTION_INVERT;
  if(strcasecmp(str, "inv"      ) == 0) return ACTION_INVERT;
  if(strcasecmp(str, "increase" ) == 0) return ACTION_INCREASE;
  if(strcasecmp(str, "increment") == 0) return ACTION_INCREASE;
  if(strcasecmp(str, "inc"      ) == 0) return ACTION_INCREASE;
  if(strcasecmp(str, "decrease" ) == 0) return ACTION_DECREASE;
  if(strcasecmp(str, "decrement") == 0) return ACTION_DECREASE;
  if(strcasecmp(str, "dec"      ) == 0) return ACTION_DECREASE;
  if(strcasecmp(str, "tri"      ) == 0) return ACTION_TRISTATE;
  if(strcasecmp(str, "tristate" ) == 0) return ACTION_TRISTATE;
  if(strcasecmp(str, "sleep"    ) == 0) return ACTION_SLEEP_SHORT;
  if(strcasecmp(str, "exec"     ) == 0) return ACTION_EXEC;
  if(strcasecmp(str, "meta"     ) == 0) return ACTION_DEFINE_META;
  if(strcasecmp(str, "down"     ) == 0) return ACTION_KEY_DOWN;
  if(strcasecmp(str, "up"       ) == 0) return ACTION_KEY_UP;
  if(strcasecmp(str, "type"     ) == 0) return ACTION_TYPE;
  if(strcasecmp(str, "boot"     ) == 0) return ACTION_BOOT;
  if(strcasecmp(str, "swap"     ) == 0) return ACTION_SWAP;
  if(strcasecmp(str, "press"    ) == 0) return ACTION_KEY_PRESS;
  if(strcasecmp(str, "using"    ) == 0) return ACTION_DEFINE_SWITCH;
  if(strcasecmp(str, "save"     ) == 0) return ACTION_SAVE_STATE;
  if(strcasecmp(str, "restore"  )  == 0) return ACTION_RESTORE_STATE;
  if(strcasecmp(str, "requires" ) == 0) return ACTION_REQUIRES;
  if(strcasecmp(str, "map"      ) == 0) return ACTION_MAP;
  if(strcasecmp(str, "version"  ) == 0) return ACTION_SHOW_VERSION;
  if(strcasecmp(str, "state"    ) == 0) return ACTION_SHOW_STATE;
  if(strcasecmp(str, "status"   ) == 0) return ACTION_SHOW_STATE;
  if(strcasecmp(str, "lock"     ) == 0) return ACTION_LOCK;
  if(strcasecmp(str, "password" ) == 0) return ACTION_SET_PASSWORD;
  if(strcasecmp(str, "memorize" ) == 0) return ACTION_MEMORIZE;
  if(strcasecmp(str, "recall"   ) == 0) return ACTION_RECALL;
  if(strcasecmp(str, "speed"    ) == 0) return ACTION_DEFINE_SPEED;
  if(strcasecmp(str, "expand"   ) == 0) return ACTION_EXPAND;
  if(strcasecmp(str, "serial"   ) == 0) return ACTION_DEFINE_SERIAL;

  return ACTION_NONE;
}

//-----------------------------------------------------------------------------

static bool parseInt(char* word, int base, uint8_t *value) {
  char *failed;
  *value = strtol(word, &failed, base);
  return (word != failed);
}

//-----------------------------------------------------------------------------

static uint8_t parseNativePort(char *str) {
  if(str == NULL) return PORT_NONE;

  if(strncasecmp(str, "a",  1) == 0) return PORT_A;
  if(strncasecmp(str, "b",  1) == 0) return PORT_B;

  return PORT_NONE;
}

//-----------------------------------------------------------------------------

static void availablePorts(void) {
  fprintf(stderr, "available ports: a, b");
  if(!Config_has_expansion(config)) {
    fprintf(stderr, "\n");
    return;
  }
  for(int i=0; i<config->expansion->num_ports; i++) {
    fprintf(stderr, ", %c", 'c' + i);
  }
  fprintf(stderr, "\n");
}

//-----------------------------------------------------------------------------

static uint8_t parsePort(char *str) {
  uint8_t port = parseNativePort(str);
  if(port != PORT_NONE) return port;

  if(!ignoreMissingPorts) {
    if(!Config_has_expansion(config)) {
      return PORT_NONE;
    }
  }

  char c = toupper(str[0]);
  if(!isalpha(c)) return PORT_NONE;

  port = c - 'A';

  if(!ignoreMissingPorts) {
    if(config->expansion->num_ports + 2 <= port) {
      return PORT_NONE;
    }
  }
  return port;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

static bool parsePolicy(Command* command, char *str) {
  bool result = true;

  if(strcmp(str, "0") == 0) {
    command->policy = POLICY_EVEN;
  }
  else if(strcmp(str, "1") == 0) {
    command->policy = POLICY_ODD;
  }
  else {
    result = false;
  }
  return result;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

static bool equal(const char *a, const char *b) {
  if(a == NULL || b == NULL) return false;
  return strcasecmp(a, b) == 0;
}

//-----------------------------------------------------------------------------

static bool ends_with(const char *str, const char *end) {
  if(str == NULL || end == NULL) return false;
  if(strlen(end) > strlen(str)) return false;
  return strncasecmp(str+strlen(str)-strlen(end), end, strlen(end)) == 0;
}

//-----------------------------------------------------------------------------

static bool isSymbol(char *name) {
    for(int i=0; i<sizeof(symbols)/sizeof(Symbol); i++) {
      if(strcasecmp(symbols[i].name, name) == 0) {
        return true;
      }
    }
    return false;
}

//-----------------------------------------------------------------------------

static bool isSymbolName(char *name) {
  return
    strspn(name,
           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
           "abcdefghijklmnopqrstuvwxyz"
           "0123456789"
           "_-+.!?*$§%&~^") == strlen(name);
}

//-----------------------------------------------------------------------------

static char* trim(char* s)
{
  int len = strlen(s);

  while (len && isspace(s[0]))
  {
    s++; len--;
  }

  while (len && isspace(s[len - 1]))
  {
    s[len - 1] = 0; len--;
  }

  return s;
}

//-----------------------------------------------------------------------------

static void unescape(char *str) {
  char *saved_str = str;
  char *input = strdup(str);
  char *saved_input = input;
  int closing;
  char in, out;

  for(input; input[0] != '\0'; input++) {
    char hex[] = { 0, 0, 0 };

    in = input[0];
    if(in == '\\') {
      switch(input[1]) {
      case '\\':
        out = '\\';
        break;
      case 'r':
        out = '\r';
        break;
      case 'n':
        out = '\n';
        break;
      case 'f':
        out = '\f';
        break;
      case 'x':
        hex[0] = input[2];
        hex[1] = input[3];
        out = strtol(hex, NULL, 16) & 0xff;
        input+=2;
        break;
      case '{':
        closing = strcspn(input+2, "}");
        input[2+closing] = '\0';
        out = strtol(input+2, NULL, 10);
        input += closing + 1;
        break;
      default:
        out = in;
        fprintf(stderr, "Warning: \\%c: undefined escape sequence in string literal (skipped)\n", input[1]);
        input++;
        continue;
      }
      str[0] = out;
      input++;
    }
    else {
      out = in;
      str[0] = out;
    }
    str++;
  }
  free(saved_input);

  str[0] = '\0';
  str = saved_str;
}

//-----------------------------------------------------------------------------

static char* escape(char* str) {
  char* output = (char*) calloc(strlen(str)*6, sizeof(char));
  char seq[7];

  for(int i=0; i<strlen(str); i++) {
    switch(str[i]) {
    case '\\':
      strcat(output, "\\\\");
      break;
    case '\n':
      strcat(output, "\\n");
      break;
    case '\r':
      strcat(output, "\\r");
      break;
    case '\f':
      strcat(output, "\\f");
      break;
    default:
      if((str[i] >= 32  && str[i] <= 127) ||
         (((unsigned char)str[i]) >= 192 && ((unsigned char)str[i]) <= 223)) {
        output[strlen(output)] = str[i];
      }
      else {
        sprintf(seq, "\\{%d}", (unsigned char)str[i]);
        strcat(output, seq);
      }
    }
  }
  return output;
}

//-----------------------------------------------------------------------------
// Functions for parsing config files
//-----------------------------------------------------------------------------

bool Config_parse(Config* self, FILE* in) {

  char* buffer = (char*) calloc(4096, sizeof(char));
  char* line;
  char* colon;
  char* equals;
  char* name;
  char* value;
  char *keyspec;
  char *comment;
  int pos = 0;
  int trailing = 0;
  uint8_t times = 0;

  Binding* binding;
  Command* command;
  Expansion* expansion;
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

    // check if this command is a definition
    if(strstr(line, "=") != NULL) {
      name = strdup(line);
      equals = strstr(name, "=");
      equals[0] = '\0';

      name = trim(name);

      if(!isSymbolName(name)) {
        goto not_a_symbol;
      }

      value = equals+1;
      value = trim(value);

      if(StringList_has_definition(name)) {
        fprintf(stderr, "error: line %d: '%s': symbol already defined\n", pos, name);
        return false;
      }

      if(isSymbol(name)) {
        fprintf(stderr, "error: line %d: '%s': symbol is a reserved key name\n", pos, name);
        return false;
      }

      if(parseAction(name) != ACTION_NONE) {
        fprintf(stderr, "error: line %d: '%s': symbol is a reserved keyword\n", pos, name);
        return false;
      }

      StringList_add_definition(name, value);
      continue;
    }

  not_a_symbol:

    // check if this command shall be bound to a key
    if((colon = strstr(line, ":")) != NULL) {
      keyspec = line;
      colon[0] = '\0';
      line = colon+1;

      // try to parse the keyspec...
      if(!Key_parse(&key, keyspec, false)) {
        fprintf(stderr, "error: line %d: '%s': invalid key or slot specification\n", pos, keyspec);
        return false;
      }
    }
    else {
      // no keyspec given -> bind to immediate "key"
      key = KEY_IMMEDIATE;
    }

    // skip leading whitespace of command spec
    line += strspn(line, ws);

    // try to parse expansion...
    expansion = Expansion_new();

    if(Expansion_parse(expansion, line)) {
      Config_set_expansion(self, expansion);
      continue;
    }
    else {
      Expansion_free(expansion);
    }

    // try to parse command...
    command = Command_new();

    if(!Command_parse(command, line)) {
      fprintf(stderr, "error: line %d: '%s': invalid command specification\n", pos, line);
      free(command);
      return false;
    }

    if(command->action == ACTION_REQUIRES) {
      Config_get_or_create_binding(self, key)->times = command->data;
    }
    else {
      Binding_add(Config_get_or_create_binding(self, key), command);
    }
  }

  free(buffer);
  return true;
}

//-----------------------------------------------------------------------------

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

    if(byte >= 0xfb) {
      fprintf(stderr, "error: '$%02X': reserved slot (use $40-$FB only)\n", byte);
      goto done;
    }

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

  if(StringList_has_definition(spec)) {
    return Key_parse(key, StringList_get_definition(spec)->value, reportUnknownSymbol);
  }

  if(!result && reportUnknownSymbol) {
    fprintf(stderr, "error: '%s': unknown key name\n", spec);
  }

 done:
  return result;
}

//-----------------------------------------------------------------------------

bool Command_parse(Command* self, char* spec) {

  bool result = true;
  bool error = false;
  StringList* words = StringList_new();
  char* word;
  char* str;
  char* failed;
  uint8_t data;
  uint16_t index;
  uint32_t value;
  bool has_policy = false;
  int i = 0;

  StringList_append_tokenized(words, spec, ws);

  if(parsePolicy(self, StringList_get(words, i))) {
    i++;
    has_policy = true;
  }

  if((self->action = parseAction(StringList_get(words, i++))) == ACTION_NONE) {
    fprintf(stderr, "error: '%s': invalid command\n", StringList_get(words, i-1));
    goto error;
  }

  if(self->action == ACTION_REQUIRES) {
    str = StringList_get(words, i++);
    self->data = strtol(str, &failed, 0);
    if(str == failed) {
      fprintf(stderr, "error: missing times for requires\n");
      goto error;
    }
    goto done;
  }

  if(self->action == ACTION_TYPE) {
    str = spec + strcspn(spec, ws) + 1;

    if(has_policy) {
      str = str + strcspn(str, ws) + 1;
    }

    unescape(str);

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

  if(self->action == ACTION_DEFINE_SPEED) {
    str = StringList_get(words, i);

    if(str == NULL) {
      fprintf(stderr, "error: missing value for 'speed' (use 'fast' or 'slow')\n");
      goto error;
    }

    if(strcasecmp(str, "fast") == 0) {
      self->data = SPEED_FAST;
      goto done;
    }

    if(strcasecmp(str, "slow") == 0) {
      self->data = SPEED_SLOW;
      goto done;
    }

    fprintf(stderr, "error: '%s': invalid value for 'speed' (use 'fast' or 'slow')\n", str);
    goto error;
  }

  if(self->action == ACTION_DEFINE_SERIAL) {
    str = StringList_get(words, i);

    if(str == NULL) {
      fprintf(stderr, "error: missing string value for 'serial'\n");
      goto error;
    }

    if(strlen(str) > 16) {
      fprintf(stderr, "error: serial string too long (max length 16): '%s'\n", str);
      goto error;
    }

    if(!Config_has_string(config, str, &index)) {
      index = Config_add_string(config, str);
    }

    self->mask = index & 0xff;
    self->data = (index >> 8) & 0xff;
    goto done;
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

  if(i >= words->size) goto done;

  if(self->action == ACTION_DEFINE_META ||
     self->action == ACTION_KEY_DOWN ||
     self->action == ACTION_KEY_UP ||
     self->action == ACTION_KEY_PRESS ||
     self->action == ACTION_EXEC) {
    goto portless;
  }

  word = StringList_get(words, i);
  if(isalpha(word[0]) && isdigit(word[1])) {
    char p[2] = { 0, 0 };
    char* b = word+1;
    p[0] = word[0];

    if((self->port = parsePort(p)) == PORT_NONE) {
      fprintf(stderr, "error: '%s': invalid line specification\n", word);
      availablePorts();
      goto error;
    }
    if(strlen(b) > 1) {
      self->mask = parseBits(b, &error);

      if(error) {
        fprintf(stderr, "error: '%s': invalid bit range\n", b);
        goto error;
      }
    }
    else {
      if((self->mask = parseBit(b)) == 0xff) {
        fprintf(stderr, "error: '%s': invalid bit\n", b);
        goto error;
      }
      self->data = 1;
    }
    i++;
  }
  else {
    if(equal(StringList_get(words, i), "port")) {
      if((self->port = parsePort(StringList_get(words, ++i))) == PORT_NONE) {
        fprintf(stderr, "error: '%s': invalid port, ", StringList_get(words, i));
        availablePorts();
        goto error;
      }
      i++;
    }

    if(equal(StringList_get(words, i), "bit")) {
      if((self->mask = parseBit(StringList_get(words, ++i))) == 0xff) {
        fprintf(stderr, "error: '%s': invalid bit\n", StringList_get(words, i));
        goto error;
      }
      self->data = 1;
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
  }

  if(equal(StringList_get(words, i), "to") || equal(StringList_get(words, i), "=")) {
    i++;
  }

 portless:

  if(i < words->size) {
    if(!parseData(StringList_get(words, i), &data)) {
      fprintf(stderr, "error: '%s': invalid value\n", StringList_get(words, i));
      goto error;
    }
    self->data = data;
  }

  if(self->port > PORT_B) {
    if(self->action == ACTION_TRISTATE) {
      fprintf(stderr, "error: expanded port lines can not be tristated\n");
      goto error;
    }
    if(self->action == ACTION_MAP) {
      fprintf(stderr, "error: expanded port lines can not be mapped to keys\n");
      goto error;
    }
  }

 done:
  StringList_free(words);
  return result;

 error:
  result = false;
  goto done;
}

//-----------------------------------------------------------------------------

bool Expansion_parse(Expansion* self, char* spec) {
  int result = false;

  char* linenames[4] = { "clock", "data", "latch", "enable" };
  uint8_t* lines[4]  = { &self->clock, &self->data, &self->latch, &self->enable };
  bool parsed[4]     = { false, false, false, false };

  StringList* words = StringList_new();
  StringList_append_tokenized(words, spec, ws);

  char *word;
  char* pair;
  char *name;
  char *equals;
  char *value;

  uint8_t num_ports;
  uint8_t port;
  uint8_t pin;
  uint8_t line;

  int i = 0;

  if(parseAction(StringList_get(words, i++)) != ACTION_EXPAND) {
    goto done;
  }

  for(i; i<words->size; i++) {
    word = StringList_get(words, i);
    pair = strdup(word);

    if((equals = strstr(pair, "=")) == NULL) {
      fprintf(stderr, "error: '%s': expected <name>=<value>\n", word);
      goto done;
    }

    name = pair;
    equals[0] = '\0';
    value = equals+1;

    if(equal(name, "ports")) {
      if(!parseInt(value, 10, &num_ports)) {
        fprintf(stderr, "error: '%s': invalid number of ports\n", value);
        goto done;
      }
      Expansion_set_num_ports(self, num_ports);
    }

    for(int k=0; k<4; k++) {
      if(equal(name, linenames[k])) {

        if(!parseInt(value+1, 10, &pin)) {
          fprintf(stderr, "error: '%s': invalid pin number\n", value);
          goto done;
        }

        if(pin > 7) {
          fprintf(stderr, "error: '%s': pin number out of range (0-7)\n", value+1);
          goto done;
        }

        value[1] = '\0';
        if((port = parseNativePort(value)) == PORT_NONE) {
          fprintf(stderr, "error: '%s': invalid port name\n", value);
          goto done;
        }

        line = port << 3;
        line |= pin;
        *(lines[k]) = line;
        parsed[k] = true;
      }
    }
  }

  for(int k=0; k<4; k++) {
    if(!parsed[k]) {
      fprintf(stderr, "error: no pin specification found for '%s'\n", linenames[k]);
      goto done;
    }
  }

  if(!self->num_ports) {
    fprintf(stderr, "error: number of available expansion ports not specified\n");
    goto done;
  }

  result = true;

 done:
  StringList_free(words);
  return result;
}

//-----------------------------------------------------------------------------
// Functions for writing binary configuration
//-----------------------------------------------------------------------------

void Config_write(Config *self, FILE *out) {
  fputc(CONFIG_MAGIC[0], out);
  fputc(CONFIG_MAGIC[1], out);

  if(Config_has_expansion(self)) {
    fputc(KEY_EXPANSION, out);
    Expansion_write(self->expansion, out);
  }

  for(int i=0; i<self->num_bindings; i++) {
    Binding_write(self->bindings[i], out);
  }
  for(int i=0; i<self->num_strings; i++) {
    fputc(KEY_STRING, out);
    fputs(self->strings[i], out);
    fputc('\0', out);
  }
  for(int i=0; i<self->num_longs; i++) {
    fputc(KEY_LONG, out);
    fputc((self->longs[i] >> 0) & 0xff, out);
    fputc((self->longs[i] >> 8) & 0xff, out);
    fputc((self->longs[i] >> 16) & 0xff, out);
    fputc((self->longs[i] >> 24) & 0xff, out);
  }
  fputc(0xffU, out);
}

//-----------------------------------------------------------------------------

void Binding_write(Binding *self, FILE* out) {
  Key_write(self->key, out);
  fputc(self->times, out);
  fputc(self->num_commands, out);

  for(int i=0; i<self->num_commands; i++) {
    Command_write(self->commands[i], out);
  }
}

//-----------------------------------------------------------------------------

void Key_write(uint8_t key, FILE* out) {
  fputc(key, out);
}

//-----------------------------------------------------------------------------

void Command_write(Command *self, FILE* out) {
  uint8_t action = self->action;
  fputc(self->action, out);
  fputc(self->policy, out);
  fputc(self->port, out);
  fputc(self->mask, out);
  fputc(self->data, out);
}

//-----------------------------------------------------------------------------

void Expansion_write(Expansion* self, FILE* out) {
  fputc(self->num_ports, out);
  fputc(self->clock, out);
  fputc(self->data, out);
  fputc(self->latch, out);
  fputc(self->enable, out);
}

//-----------------------------------------------------------------------------
// Functions for writing canonical config file format
//-----------------------------------------------------------------------------

void Config_print(Config *self, FILE* out) {
  if(Config_has_expansion(self)) {
    Expansion_print(self->expansion, out);
  }
  for(int i=0; i<self->num_bindings; i++) {
    Binding_print(self->bindings[i], out);
  }
}

//-----------------------------------------------------------------------------

void Binding_print(Binding *self, FILE* out) {

  if(self->times) {
    if(self->key != KEY_IMMEDIATE) {
      Key_print(self->key, out);
    }
    fprintf(out, "requires %dx\n", self->times);
  }

  for(int i=0; i<self->num_commands; i++) {
    if(self->key != KEY_IMMEDIATE) {
      Key_print(self->key, out);
    }
    Command_print(self->commands[i], out);
  }
}

//-----------------------------------------------------------------------------

void Key_print(uint8_t key, FILE* out) {
  fprintf(out, "$%02X: ", key);
}

//-----------------------------------------------------------------------------

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
  case ACTION_SAVE_STATE:    action = "save";     break;
  case ACTION_RESTORE_STATE: action = "restore";  break;
  case ACTION_REQUIRES:      action = "requires"; break;
  case ACTION_MAP:           action = "map";      break;
  case ACTION_SHOW_VERSION:  action = "version";  break;
  case ACTION_SHOW_STATE:    action = "state";    break;
  case ACTION_LOCK:          action = "lock";     break;
  case ACTION_SET_PASSWORD:  action = "password"; break;
  case ACTION_MEMORIZE:      action = "memorize"; break;
  case ACTION_RECALL:        action = "recall";   break;
  case ACTION_DEFINE_SPEED:  action = "speed";    break;
  case ACTION_DEFINE_SERIAL: action = "serial";   break;
  };

  if(self->policy == POLICY_EVEN) {
    fprintf(out, "0 ");
  }

  if(self->policy == POLICY_ODD) {
    fprintf(out, "1 ");
  }

  if(self->action == ACTION_TYPE) {
    uint16_t index = self->mask | (self->data << 8);
    char *escaped = escape(config->strings[index]);
    fprintf(out, "%s %s\n", action, escaped);
    free(escaped);
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

  if(self->action == ACTION_BOOT ||
     self->action == ACTION_SAVE_STATE ||
     self->action == ACTION_RESTORE_STATE) {
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

  else if(self->action == ACTION_DEFINE_SPEED) {
    fprintf(out, "%s %s", action,
            self->data == SPEED_SLOW ? "slow" : "fast");
  }

  else if(self->action == ACTION_DEFINE_SERIAL) {
    uint16_t index = self->mask | (self->data << 8);
    char *serial = config->strings[index];
    fprintf(out, "%s %s\n", action, serial);
    return;
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

      fprintf(out, "%s port %c bit %d ",
              action,
              'a' + self->port,
              start);
    } else {

      fprintf(out, "%s port %c bits %d-%d ",
              action,
              'a' + self->port,
              start,
              end);
    }
  }
  if(self->action == ACTION_SET || self->action == ACTION_MAP) {
    fprintf(out, "$%02X", self->data);
  }
  fprintf(out, "\n");
}

//-----------------------------------------------------------------------------

void Expansion_print(Expansion* self, FILE* out) {
  fprintf(out,
          "expand PORTS=%d "
          "CLOCK=%s%d "
          "DATA=%s%d "
          "LATCH=%s%d "
          "ENABLE=%s%d\n",
          self->num_ports,
          (self->clock & 0x08) ? "b" : "a", self->clock & 0x07,
          (self->data & 0x08) ? "b" : "a", self->data & 0x07,
          (self->latch & 0x08) ? "b" : "a", self->latch & 0x07,
          (self->enable & 0x08) ? "b" : "a", self->enable & 0x07);
}

//-----------------------------------------------------------------------------

#if (defined(WIN32) && !defined(__CYGWIN__)) || defined(__APPLE__)
FILE* fmemopen(void *__restrict buf, size_t size, const char *__restrict mode) {
  FILE* result;
  int fd;
  char path[4096];
  char file[4096] = "keyman64-XXXXXX";

#if defined(WIN32)
  if(!GetTempPath(4096, path)) return NULL;
  if(!GetTempFileName(path, "key", 0, file)) return NULL;
  result = fopen(file, "wbD+");
#else
  fd = mkstemp(file);
  result = fdopen(fd, "w+");
#endif

  fwrite(buf, sizeof(char), size, result);
  fseek(result, 0, SEEK_SET);

  return result;
}
#endif

void fmemupdate(FILE *fp, void *buf,  uint16_t size) {
#if (defined(WIN32) && !defined(__CYGWIN__)) || defined(__APPLE__)
  fseek(fp, 0, SEEK_SET);
  fread(buf, sizeof(uint8_t), size, fp);
#endif
}

//-----------------------------------------------------------------------------

static void prepare_devices(void) {
  strncpy(keyman64.path, device, 4096);
  strncpy(keyman64.role, "Keyman64", 64);
  keyman64.vid = KEYMAN64_VID;
  keyman64.pid = KEYMAN64_PID;

#if linux
  strncpy(usbasp.path, "/dev/usbasp", 4096);
#else
  strncpy(usbasp.path, "usbasp", 4096);
#endif
  strncpy(usbasp.role, "Bootloader", 64);
  usbasp.vid = USBASP_VID;
  usbasp.pid = USBASP_PID;
}

//-----------------------------------------------------------------------------

void print_serial(libusb_device *device, libusb_device_descriptor_t *descriptor) {
  char serial[16+1];

  usb_get_serial_number(device, descriptor, serial, 16);

  if(strlen(serial) > 0) {
    printf("%s ", serial);
  }
}

//-----------------------------------------------------------------------------

void complete_devices() {
  usb_iterate_devices(NULL, KEYMAN64_VID, KEYMAN64_PID, (void*) &print_serial);
}

//-----------------------------------------------------------------------------

void complete_options() {
  extern struct option options[];
  struct option option;
  int i = 0;

  while(true) {
    option = options[i];
    if(option.name == 0) break;

    if(strstr(option.name, "complete") == NULL) {
      printf("--%s ", option.name);
    }

    i++;
  }
}

//-----------------------------------------------------------------------------

struct option options[] = {
  { "help",              no_argument,       0, 'h' },
  { "version",           no_argument,       0, 'v' },
  { "device",            required_argument, 0, 'd' },
  { "keys",              no_argument,       0, 'k' },
  { "delay",             required_argument, 0, 'D' },
  { "preserve",          no_argument,       0, 'p' },
  { "identify",          no_argument,       0, 'i' },
  { "complete-options",  no_argument,       0, '0' },
  { "complete-devices",  no_argument,       0, '1' },
  { 0, 0, 0, 0 },
};

//-----------------------------------------------------------------------------

int main(int argc, char **argv) {
  int result = EXIT_SUCCESS;
  char* env_device;

  device = calloc(1, sizeof(char));
  device[0] = '\0';

  usb_quiet = false;

  if((env_device = getenv("KEYMAN64_DEVICE")) != NULL) {
    device = (char*) realloc(device, (strlen(env_device)+1) * sizeof(char));
    strcpy(device, env_device);
  }

  prepare_devices();

  int option, option_index;

  while(1) {
    option = getopt_long(argc, argv, "hvd:kD:pi01", options, &option_index);

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

    case 'p':
      fprintf(stderr,
              "hint: option --preserve is no longer needed "
              "and has been deprecated in v1.5\n");
      break;

    case 'i':
      identify();
      goto done;
      break;

    case '0':
      complete_options();
      goto done;
      break;

    case '1':
      complete_devices();
      goto done;
      break;

    case '?':
    case ':':
      goto done;
    }
  }

  argc -= optind;
  argv += optind;

  if(!argc) {
    usage();
    complain();
    goto done;
  }
  prepare_devices();

  if(argc && (strcmp(argv[0], "convert") == 0)) {
    result = convert(--argc, ++argv);
  }
  else if(argc && (strcmp(argv[0], "configure") == 0)) {
    result = configure(--argc, ++argv);
  }
  else if(argc && (strcmp(argv[0], "update") == 0)) {
    result = update(--argc, ++argv);
  }
  else if(argc && (strcmp(argv[0], "reset") == 0)) {
    result = reset();
  }
  else {
    result = command(argc, argv);
  }

 done:
  free(device);
  return result;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

int command(int argc, char **argv) {
  int result = EXIT_FAILURE;

  FILE *in = NULL;
  FILE *out = NULL;
  char *str = (char*) calloc(1, sizeof(char));
  uint8_t *data = NULL;
  uint16_t size = 0;

  ignoreMissingPorts = true;

  if(strlen(argv[0]) == 1 && argv[0][0] == '-') {
    in = stdin;
  }
  else if((in = fopen(argv[0], "rb")) == NULL) {

    join(&str, argv, argc);

    if(strlen(str)) {
      if((in = fmemopen(str, strlen(str)+1, "rb")) == NULL) {
        fprintf(stderr, "error: could not open string via fmemopen(): %s \n", strerror(errno));
        goto done;
      }
    }
  }

  if(in == NULL) {
    goto done;
  }

  if(in == stdin) {
    fprintf(stderr, "reading commands from stdin...\n");
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

  if((result = usb_send(&keyman64, KEYMAN64_CTRL, delay, 0, data, size)) < 0) {
    fprintf(stderr, "error: could send usb control message: %s\n", libusb_strerror(result));
    goto done;
  }
  result = EXIT_SUCCESS;

 done:
  if(config != NULL) {
    Config_free(config);
  }

  if(data != NULL) {
    free(data);
  }
  free(str);
  return result;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

int configure(int argc, char **argv) {

  int result = false;

  FILE *in  = stdin;
  FILE *out = NULL;
  uint8_t *data = NULL;
  uint16_t size = 0;

  config = Config_new();

  if(argc >= 1 && (strncmp(argv[0], "-", 1) != 0)) {

    if((in = fopen(argv[0], "rb")) == NULL) {
      fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
      goto done;
    }
  }

  if(in == stdin) {
    fprintf(stderr, "reading from stdin...\n");
  }

  if(Config_read(config, in) || Config_parse(config, in)) {

    data = (uint8_t*) calloc(4096, sizeof(char));

    if((out = fmemopen(data, 4096, "wb")) == NULL) {
      fprintf(stderr, "error: %s\n", strerror(errno));
      goto done;
    }

    Config_write(config, out);
    size = ftell(out);
    fmemupdate(out, data, size);
    fclose(out);

    if(!usb_ping(&keyman64)) {
      fprintf(stderr, "error: could not connect to keyman64\n");
      goto done;
    }

    fprintf(stderr, "Flashing configuration: %d bytes...", size);
    fflush(stderr);

    result = usb_send(&keyman64, KEYMAN64_FLASH, 0, 0, data, size) == size;
    fprintf(stderr, result ? "ok\n" : "failed!\n");
  }

 done:
  Config_free(config);
  fclose(in);

  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

//-----------------------------------------------------------------------------

int update(int argc, char **argv) {
  int result = EXIT_FAILURE;

  char *filename = argv[0];
  unsigned int address = 0;
  uint8_t *data = (uint8_t *) calloc(1, sizeof(uint8_t));
  int size = 0;

  if(!argc) {
    usage();
    goto done;
  }

  if(!(result = read_file(filename, &data, &size))) {
    goto done;
  }

  if(ends_with(filename, ".hex")) {
    fprintf(stderr, "Trying to parse Intel HEX format..."); fflush(stderr);

    data = readhex(data, &size, &address);

    if(!data) {
      fprintf(stderr, "FAILED!\n");
      errno = EINVAL;
      goto error;
    }
    fprintf(stderr, "OK\nParsed %d bytes starting at 0x%02X\n", size, address);
  }
  else if(!ends_with(filename, ".bin")) {
    fprintf(stderr, "error: please supply firmware as a .bin or .hex file\n");
    goto done;
  }

  if(argc == 2) {
    uint8_t erased[2] = { 0x00, 0x00 };

    if(!usb_ping(&keyman64)) {
      fprintf(stderr, "error: could not connect to keyman64\n");
      goto done;
    }

    fprintf(stderr, "Deactivating existing configuration...");
    fflush(stderr);

    result = usb_send(&keyman64, KEYMAN64_FLASH, 0, 0, erased, 2) == 2;
    fprintf(stderr, result ? "ok\n" : "failed!\n");
    if(!result) goto done;

    expect(&keyman64, "Waiting for keyman64 to reboot");
  }

  if(usb_ping(&keyman64)) {
    usb_control(&keyman64, KEYMAN64_BOOT);
    expect(&usbasp, "Entering bootloader");
  }

  if(usb_ping(&usbasp)) {
    usb_control(&usbasp, USBASP_CONNECT);

    for(uint32_t i=0; i<size+64; i+=64) {
      usb_send(&usbasp, USBASP_WRITEFLASH,
               (uint16_t) (i & 0xffff), (uint16_t) (i>>16),
               data+i, 64);
      fprintf(stderr, "\rUpdating application: %d of %d bytes transferred...",
              (i<size) ? i : size, size);
    }
    fprintf(stderr, "ok\n");

    usb_quiet = true;
    usb_control(&usbasp, USBASP_DISCONNECT);
  }
  else {
    fprintf(stderr, "error: could not connect to usbasp bootloader\n");
  }

  if(argc == 2) {
    expect(&keyman64, "Waiting for keyman64 to reboot");
    argc--; argv++;
    configure(argc, argv);

    if(expect(&keyman64, "Waiting for keyman64 to reboot")) {
      identify();
    }
  }

 done:
  if(data != NULL) free(data);
  return result;

 error:
  fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
  goto done;
}

//-----------------------------------------------------------------------------

bool expect(DeviceInfo *device, const char* message) {

  fprintf(stderr, "%s", message); fflush(stderr);

  uint8_t tries = 10;
  usb_quiet = true;

  do {
    sleep(1);
    fprintf(stderr, "."); fflush(stderr);

    if(!--tries) {
      return false;
    }
  } while(!usb_ping(device));

  fprintf(stderr, "\r");
  for(int i=0; i<strlen(message)+10; i++) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "\r");
  return true;
}

//-----------------------------------------------------------------------------

bool read_file(char* filename, uint8_t **data, int *size) {

  bool result = false;
  FILE *in = NULL;

  struct stat st;

  if((in = fopen(filename, "rb")) == NULL) {
    goto error;
  }

  if(fstat(fileno(in), &st) == -1) {
    goto error;
  }

  (*size) = st.st_size;
  (*data) = realloc((*data), (*size));

  if(fread((*data), sizeof(uint8_t), (*size), in) != (*size)) {
    goto error;
  }
  fclose(in);

  result = true;

 done:
  return result;

 error:
  fprintf(stderr, "%s: %s\n", filename, strerror(errno));
  goto done;
}

//-----------------------------------------------------------------------------

void identify(void) {
  char id[64];
  prepare_devices();
  if(usb_receive(&keyman64, KEYMAN64_IDENTIFY, 0, 0, (uint8_t*) id, 64) > 0) {
    printf("%s\n", id);
  }
}

//-----------------------------------------------------------------------------

bool reset(void) {

  if(usb_ping(&keyman64)) {
    usb_control(&keyman64, KEYMAN64_RESET);
  }
  else if(usb_ping(&usbasp)) {
    usb_quiet = true;
    usb_control(&usbasp, USBASP_CONNECT);
    usb_control(&usbasp, USBASP_DISCONNECT);
  }
  else {
    failed(&keyman64);
    failed(&usbasp);
    return false;
  }

  if(expect(&keyman64, "Resetting device")) {
    identify();
    return true;
  }
  failed(&keyman64);
  return false;
}

//-----------------------------------------------------------------------------

void failed(DeviceInfo *device) {
  if(strlen(device->path)) {
      fprintf(stderr, "error: failed to open %s device \"%s\" (%04X:%04X)\n",
              device->role, device->path, device->vid, device->pid);
  }
  else {
      fprintf(stderr, "error: failed to open %s device (%04X:%04X)\n",
              device->role, device->vid, device->pid);
  }
}

//-----------------------------------------------------------------------------

void version(void) {
  printf("Keyman64 %s\n", VERSION);
  printf("Copyright (C) 2016 Henning Bekel.\n");
  printf("License GPLv3: GNU GPL version 3 <http://gnu.org/licenses/gpl.html>.\n");
  printf("This is free software: you are free to change and redistribute it.\n");
  printf("There is NO WARRANTY, to the extent permitted by law.\n");
}

//-----------------------------------------------------------------------------

void usage(void) {
  version();
  printf("\n");
  printf("Usage:\n");
  printf("      keyman64 [<options>] convert [<infile>|-] [<outfile>|-]\n");
  printf("      keyman64 [<options>] configure [<infile>]\n");
  printf("      keyman64 [<options>] update <firmware> [<config>]\n");
  printf("      keyman64 [<options>] reset\n");
  printf("      keyman64 [<options>] <command>\n");
  printf("      keyman64 [<options>] <script>|-\n");
  printf("\n");
  printf("  Options:\n");
  printf("           -v, --version  : print version information\n");
  printf("           -h, --help     : print this help text\n");
#if linux
  printf("           -d, --device   : specify usb device (default: /dev/keyman64)\n");
#elif windows
  printf("           -d, --device   : specify usb device (default: usb)\n");
#endif
  printf("           -D, --delay    : delay in ms between commands\n");
  printf("           -k, --keys     : list key names and synonyms\n");
  printf("           -p, --preserve : deprecated as of version 1.5\n");
  printf("           -i, --identify : request firmware identification via USB\n");
  printf("\n");
  printf("  Files:\n");
  printf("           <infile>   : input file, format is autodetected\n");
  printf("           <outfile>  : output file, format determined by extension\n");
  printf("           <script>   : script file containing keyman64 commands\n");
  printf("           <firmware> : binary or ihex firmware image (.bin or .hex)\n");
  printf("\n");
  printf("           *.conf : plain text config file format\n");
  printf("           *.bin  : binary file format (default)\n");
  printf("\n");
  printf("           Optional arguments default to stdin or stdout\n");
  printf("\n");
  printf("  Command:\n");
  printf("            (any valid keyman64 command)\n");
  printf("\n");
}

//-----------------------------------------------------------------------------

void complain(void) {
#if defined(WIN32) && !defined(__CYGWIN__)
  if(!(!GetConsoleTitle(NULL, 0) && GetLastError() == ERROR_SUCCESS)) {
    printf("\n!THIS IS A COMMANDLINE APPLICTION, PLEASE RUN "
           "IT FROM A COMMAND PROMPT INSTEAD!\n\n");
    system("pause");
  }
#endif
}

//-----------------------------------------------------------------------------

void keys(void) {
  Symbol symbol;
  Symbol synonym;
  int total = sizeof(symbols)/sizeof(Symbol);

  for(int i=0; i<64; i++) {
    printf("$%02X %2d   ", i, i);
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

//-----------------------------------------------------------------------------

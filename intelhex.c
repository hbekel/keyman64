#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "intelhex/kk_ihex.h"
#include "intelhex/kk_ihex_read.h"
#include "intelhex/kk_ihex_read.c"
#include "intelhex/kk_ihex_write.h"
#include "intelhex/kk_ihex_write.c"

static uint8_t* _buffer = NULL;
static int _size = 0;
static FILE* _out;
static unsigned int _address;
static bool successful;
//------------------------------------------------------------------------------

uint8_t* readhex(uint8_t *data, int *size, unsigned int *address) {
  
  _buffer = (uint8_t*) calloc(0x20000, sizeof(uint8_t));
  memset(_buffer, 0xff, 0x20000);
  
  _size = 0;
  _address = UINT_MAX;
  successful = false;
  
  struct ihex_state ihex;
  ihex_begin_read(&ihex);
  ihex_read_bytes(&ihex, (const char*) data, (*size));
  ihex_end_read(&ihex);

  if(successful) {
    if(data) free(data);
    (*size) = _size;
    (*address) = _address;
    return _buffer;
  }
  return NULL;
}

//------------------------------------------------------------------------------

ihex_bool_t ihex_data_read (struct ihex_state *ihex,
                            ihex_record_type_t type,
                            ihex_bool_t checksum_error) {

  if(type == IHEX_DATA_RECORD) {
    unsigned long address = (unsigned long) IHEX_LINEAR_ADDRESS(ihex);

    if(address < _address) {
      _address = address;
    }
    
    for(int i=0; i<ihex->length; i++) {
      _buffer[address+i] = ihex->data[i];
    }
    _size = address - _address + ihex->length;
  }
  else if(type == IHEX_END_OF_FILE_RECORD) {
    successful = true;
  }
  return true;
}

//------------------------------------------------------------------------------

void writehex(uint8_t *data, int size, unsigned int address, FILE* out) {
  struct ihex_state ihex;
  _out = out;
  
  ihex_init(&ihex);
  ihex_write_at_address(&ihex, address);
  ihex_write_bytes(&ihex, (const char*) data, size);
  ihex_end_write(&ihex);
}

//------------------------------------------------------------------------------

void ihex_flush_buffer(struct ihex_state *ihex, char *buffer, char *eptr) {
  int len = eptr - buffer;
  fwrite(buffer, sizeof(char), len, _out);
  fflush(_out);
}

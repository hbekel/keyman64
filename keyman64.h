#ifndef KEYMAN64_H
#define KEYMAN64_H

#include <stdio.h>
#include "target.h"

#if windows
 #include <windows.h>
#endif

#define USBASP_CONNECT    1
#define USBASP_WRITEFLASH 6
#define USBASP_READFLASH  4
#define USBASP_DISCONNECT 2

void version(void);
void identify(void);
void complain(void);
void usage(void);
void keys(void);

int convert(int argc, char **argv);
int configure(int argc, char **argv);
int update(int argc, char **argv);
int command(int argc, char **argv);

bool read_file(char* filename, uint8_t **data, int *size);

bool Config_parse(Config* self, FILE* in);
void Config_write(Config *self, FILE* out);
void Config_print(Config *self, FILE* out);

void Binding_write(Binding *self, FILE* out);
void Binding_print(Binding *self, FILE* out);

bool Key_parse(uint8_t *key, char* spec, bool reportUnknownSymbol);
void Key_write(uint8_t key, FILE* out);
void Key_print(uint8_t key, FILE* out);

bool Command_parse(Command* self, char* spec);
void Command_write(Command *self, FILE* out);
void Command_print(Command *self, FILE* out);

void State_write(State* self, FILE* out);

#if !posix
  unsigned int sleep(unsigned int sec) { Sleep(sec*1000); return 0; }
#endif

#endif // KEYMAN64_H

#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H

#include <stdio.h>

void version(void);
void usage(void);
void keys(void);

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

#endif // INTERCEPTOR_H

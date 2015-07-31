#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H

#include <stdio.h>

void Config_write(Config *self, FILE* out);
void Config_print(Config *self, FILE* out);

void Binding_write(Binding *self, FILE* out);
void Binding_print(Binding *self, FILE* out);

void Key_write(Key *self, FILE* out);
void Key_print(Key *self, FILE* out);

void Command_write(Command *self, FILE* out);
void Command_print(Command *self, FILE* out);

#endif // INTERCEPTOR_H

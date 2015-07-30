#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H

void Config_write(Config *self, void (*write) (uint8_t byte));
void Config_debug(Config *self);

void Binding_write(Binding *self, void (*write) (uint8_t byte));
void Binding_debug(Binding *self);

void Key_write(Key *self, void (*write) (uint8_t byte));
void Key_debug(Key *self);

void Command_write(Command *self, void (*write) (uint8_t byte));
void Command_debug(Command *self);

#endif // INTERCEPTOR_H

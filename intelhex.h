#ifndef INTELHEX_H
#define INTELHEX_H

uint8_t* readhex(uint8_t *data, int *size, unsigned int *address);
uint8_t* writehex(uint8_t *data, int size, unsigned int address, FILE* out);

#endif // INTELHEX_H

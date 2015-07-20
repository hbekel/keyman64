#ifndef UART_H
#define UART_H

void InitUART(void);
void WriteUART(char, FILE*);

FILE uartout = FDEV_SETUP_STREAM(WriteUART, NULL, _FDEV_SETUP_WRITE);

#endif // UART_H

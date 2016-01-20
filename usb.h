#ifndef KEYMAN64_USB_H
#define KEYMAN64_USB_H

#include <libusb-1.0/libusb.h>

#define KEYMAN64_VID 0x1d50
#define KEYMAN64_PID 0x60e9

typedef struct {
  unsigned int vid;
  unsigned int pid;
  int bus;
  int address;
  char *serial;
} DeviceInfo;

int usb_send(char* device, uint8_t message, uint16_t value, uint8_t* buf, uint16_t size);
int usb_receive(char* device, uint8_t message, uint16_t value, uint8_t* buf, uint16_t size);

#endif // KEYMAN64_USB_H

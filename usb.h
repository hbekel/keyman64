#ifndef KEYMAN64_USB_H
#define KEYMAN64_USB_H

#include <libusb-1.0/libusb.h>

typedef struct {
  unsigned int vid;
  unsigned int pid;
  int bus;
  int address;
  char *serial;
} DeviceInfo;

bool usb_lookup(char *path, DeviceInfo *info);
libusb_device_handle* usb_open(libusb_context* context, DeviceInfo *info);
int usb_send(libusb_device_handle *handle, uint8_t message, uint16_t value, uint8_t* buf, uint16_t size);
int usb_receive(libusb_device_handle *handle, uint8_t message, uint16_t value, uint8_t* buf, uint16_t size);

#endif // KEYMAN64_USB_H

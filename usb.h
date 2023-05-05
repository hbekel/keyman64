#ifndef KEYMAN64_USB_H
#define KEYMAN64_USB_H

#include <libusb-1.0/libusb.h>

#define KEYMAN64_VID 0x1d50
#define KEYMAN64_PID 0x60e9

#define USBASP_VID 0x16c0
#define USBASP_PID 0x05dc

extern bool usb_quiet;

typedef struct {
  char path[4096+1];
  char role[64];
  unsigned int vid;
  unsigned int pid;
  int bus;
  int address;
} DeviceInfo;

typedef struct libusb_device_descriptor libusb_device_descriptor_t;

bool usb_ping(DeviceInfo *info);
int usb_control(DeviceInfo *info, uint8_t message);
int usb_send(DeviceInfo *info, uint8_t message, uint16_t value, uint16_t index, uint8_t* buf, uint16_t size);
int usb_receive(DeviceInfo* info, uint8_t message, uint16_t value, uint16_t index, uint8_t* buf, uint16_t size);

void usb_iterate_devices(libusb_context* context, uint16_t vid, uint16_t pid,
                         void (*consumer(libusb_device*, libusb_device_descriptor_t *descriptor)));

void usb_get_serial_number(libusb_device *device, libusb_device_descriptor_t *descriptor, char *serial, size_t len);

#endif // KEYMAN64_USB_H

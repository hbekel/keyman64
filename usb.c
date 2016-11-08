#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "usb.h"
#include "target.h"

//------------------------------------------------------------------------------
// USB device discovery & handling
//------------------------------------------------------------------------------

static bool usb_lookup(char *path, DeviceInfo *info) {

  info->bus     = -1;
  info->address = -1;
  info->serial  = NULL;
  
#if linux

  char* real = realpath(path, NULL);
  char* tmp = real;
  char* bus;
  char* address;

  if(real == NULL) return false;
  
  char prefix[] = "/dev/bus/usb/";
  
  if(strstr(real, prefix) == real) {

    tmp += strlen(prefix);
    tmp[3] = '\0';

    bus = tmp;
    address = tmp+4;

    info->bus = strtol(bus, NULL, 10);
    info->address = strtol(address, NULL, 10);
  }
  
  free(real);

#elif windows

  char *colon;
  if((colon = strstr(path, ":")) != NULL) {
    info->serial = colon+1;
  }
#endif
  return true;
}

//------------------------------------------------------------------------------

static libusb_device_handle* usb_open(libusb_context* context, DeviceInfo *info) {

  libusb_device **devices;
  libusb_device *device;
  struct libusb_device_descriptor descriptor;
  libusb_device_handle* handle = NULL;
  
  char serial[256];
  int result;
  int i = 0;

  if((result = libusb_get_device_list(context, &devices)) < 0) {
    fprintf(stderr, "error: could not get usb device list: %s\n", libusb_strerror(result));
    return NULL;
  }
  
  while ((device = devices[i++]) != NULL) {

    if((result = libusb_get_device_descriptor(device, &descriptor)) < 0) {
      fprintf(stderr, "warning: could not get usb device descriptor: %s\n", libusb_strerror(result));
      continue;
    }
    
    if(descriptor.idVendor == info->vid &&
       descriptor.idProduct == info->pid) {
      
      if(info->bus > -1) {
        if(libusb_get_bus_number(device) != info->bus) {
          continue;
        }
      }
      
      if(info->address > -1) {
        if(libusb_get_device_address(device) != info->address) {
          continue;
        }
      }
      
      if(info->serial != NULL) {
        
        if(descriptor.iSerialNumber != 0) {
          
          if((result = libusb_open(device, &handle)) < 0) {
            fprintf(stderr, "warning: could not open usb device %03d/%03d: %s\n",
                    libusb_get_bus_number(device),
                    libusb_get_device_address(device),
                    libusb_strerror(result));
            continue;
          }
          
          result = libusb_get_string_descriptor_ascii(handle, descriptor.iSerialNumber,
                                                      (unsigned char *) &serial, sizeof(serial));
          
          if(result < 0) {
            fprintf(stderr, "warning: could not get serial number from device: %s\n",
                    libusb_strerror(result));
            goto skip;
          }
          
          if(strcmp(serial, info->serial) == 0) {
            goto done;
          }
          
        skip:
          libusb_close(handle);
          handle = NULL;
          continue;
        }
      }
      
      if(handle == NULL) {
        if((result = libusb_open(device, &handle)) < 0) {
          fprintf(stderr, "error: could not open usb device %03d/%03d: %s\n",
                  libusb_get_bus_number(device),
                  libusb_get_device_address(device),
                  libusb_strerror(result));
          
          handle = NULL;
        }
      }
      goto done;
    }
  }
  
 done:
  libusb_free_device_list(devices, true);

  return handle;
}

//------------------------------------------------------------------------------

static int usb_message(char* device, int direction, uint8_t message, uint16_t value, uint8_t* buf, uint16_t size) {

  libusb_device_handle *handle = NULL;
  DeviceInfo info;
  int result;
  
  if((result = libusb_init(NULL)) < 0) {
    fprintf(stderr, "error: could not initialize libusb-1.0: %s\n", libusb_strerror(result));
    goto done;
  }
  
  info.vid = KEYMAN64_VID;
  info.pid = KEYMAN64_PID;

  usb_lookup(device, &info);
  handle = usb_open(NULL, &info);
  
  if(handle == NULL) {
    result = -1;
    fprintf(stderr, "error: could not open usb device \"%s\"\n", device);
    goto done;
  }

  result = libusb_control_transfer(handle,
                                   LIBUSB_REQUEST_TYPE_VENDOR |
                                   LIBUSB_RECIPIENT_DEVICE |
                                   direction,
                                   message, value, 0,
                                   buf, size, 5000);
  
  if(!result) {
    fprintf(stderr, "error: could send usb control message: %s\n", libusb_strerror(result));
    goto done;
  }

 done:
  if(handle != NULL) {
    libusb_close(handle);
  }
  libusb_exit(NULL);
  
  return result;    
}

//------------------------------------------------------------------------------
// USB utility functions
//------------------------------------------------------------------------------

int usb_send(char* device, uint8_t message, uint16_t value, uint8_t* buf, uint16_t size) {
  return usb_message(device, LIBUSB_ENDPOINT_OUT, message, value, buf, size);  
}

//------------------------------------------------------------------------------

int usb_receive(char* device, uint8_t message, uint16_t value, uint8_t* buf, uint16_t size) {
  return usb_message(device, LIBUSB_ENDPOINT_IN, message, value, buf, size); 
}

//------------------------------------------------------------------------------

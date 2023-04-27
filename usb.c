#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "usb.h"
#include "target.h"

//-----------------------------------------------------------------------------
// USB device discovery & handling
//-----------------------------------------------------------------------------

bool usb_quiet = false;

//-----------------------------------------------------------------------------

#if unix
static libusb_device_handle* usb_open_dev_node(libusb_context* context, DeviceInfo *info) {

  libusb_device_handle* handle = NULL;
  libusb_device *device;
  int result;
  int fd;

  if((fd = open(info->path, O_RDWR)) < 0) {
    fprintf(stderr, "error: could not open device node %s: %s\n", info->path, strerror(errno));
    goto done;
  }

  if((result = libusb_wrap_sys_device(context, fd, &handle))) {
    fprintf(stderr, "error: libusb_wrap_sys_device(): could not obtain usb device handle: %s\n",
            libusb_strerror(result));
    close(fd);
    goto done;
  }
  close(fd);

  if((device = libusb_get_device(handle)) == NULL) {
    fprintf(stderr, "error: libusb_get_device(): could not get usb device: %s\n",
            libusb_strerror(result));
    goto done;
  }

  if((result = libusb_open(device, &handle)) != 0) {
    fprintf(stderr, "error: libusb_open(): could not open usb device: %s\n",
            libusb_strerror(result));
  }

 done:
  return handle;
}
#endif

//-----------------------------------------------------------------------------

static libusb_device_handle* usb_open(libusb_context* context, DeviceInfo *info) {

  libusb_device_handle* handle = NULL;
  libusb_device *device;
  libusb_device **devices;
  struct libusb_device_descriptor descriptor;

  char serial[256];
  int result;
  int i = 0;

#if unix // check if info->path points to a device node, else continue
  if(strlen(info->path) > 0) {

    struct stat st;
    stat(info->path, &st);

    if(S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode)) {
      if((handle = usb_open_dev_node(context, info)) != NULL) {
        return handle;
      }
    }
  }
#endif

  if(strlen(info->path) == 0) { // no specific device requested, so just open the first keyman64 device
    return libusb_open_device_with_vid_pid(context, info->vid, info->pid);
  }

  // else iterate device tree, match vid, pid and serial

  if((result = libusb_get_device_list(context, &devices)) < 0) {
    if(!usb_quiet) {
      fprintf(stderr, "error: could not get usb device list: %s\n",
              libusb_strerror(result));
    }
    return NULL;
  }

  while ((device = devices[i++]) != NULL) {

    if((result = libusb_get_device_descriptor(device, &descriptor)) < 0) {
      if(!usb_quiet) {
        fprintf(stderr, "warning: could not get usb device descriptor: %s\n",
                libusb_strerror(result));
      }
      continue;
    }

    if(descriptor.idVendor == info->vid &&
       descriptor.idProduct == info->pid) {

      if(descriptor.iSerialNumber != 0) { // this only means the device *has* a serial number

        // we have to open the device to actually get the serial number
        if((result = libusb_open(device, &handle)) < 0) {
          if(!usb_quiet) {
            fprintf(stderr, "warning: could not open usb device %03d/%03d: %s\n",
                    libusb_get_bus_number(device),
                    libusb_get_device_address(device),
                    libusb_strerror(result));
          }
          continue;
        }

        // device is opened, get the serial number
        result = libusb_get_string_descriptor_ascii(handle, descriptor.iSerialNumber,
                                                    (unsigned char *) &serial, sizeof(serial));

        if(result < 0) {
          if(!usb_quiet) {
            fprintf(stderr, "warning: could not get serial number from device: %s\n",
                    libusb_strerror(result));
          }
          // we couldn't get the serial number for some reason, so we
          // close this device again and continue with the next device
          libusb_close(handle);
          handle = NULL;
          continue;
        }

        if(strcmp(serial, info->path) == 0) { // serial matches, device is already open, so we're done
          goto done;
        }
      }
    }
  }

 done:
  libusb_free_device_list(devices, true);

  return handle;
}

//-----------------------------------------------------------------------------

static int usb_message(DeviceInfo *info, int direction, uint8_t message, uint16_t value, uint16_t index, uint8_t* buf, uint16_t size) {

  libusb_device_handle *handle = NULL;
  int result;

  if((result = libusb_init(NULL)) < 0) {
    if(!usb_quiet) {
      fprintf(stderr, "error: could not initialize libusb-1.0: %s\n",
              libusb_strerror(result));
    }
    goto done;
  }

  handle = usb_open(NULL, info);

  if(handle == NULL) {
    result = -1;
    if(!usb_quiet) {
      fprintf(stderr, "error: could not open usb device \"%s\"\n", info->path);
    }
    goto done;
  }

  result = libusb_control_transfer(handle,
                                   LIBUSB_REQUEST_TYPE_VENDOR |
                                   LIBUSB_RECIPIENT_DEVICE |
                                   direction,
                                   message, value, index,
                                   buf, size, 5000);

  if(result < 0) {
    if(!usb_quiet) {
      fprintf(stderr, "error: could not send usb control message: %s\n",
              libusb_strerror(result));
    }
    goto done;
  }

 done:
  if(handle != NULL) {
    libusb_close(handle);
  }
  libusb_exit(NULL);

  return result;
}

//-----------------------------------------------------------------------------
// USB utility functions
//-----------------------------------------------------------------------------

bool usb_ping(DeviceInfo *info) {

  libusb_device_handle *handle = NULL;
  bool result = false;

  if(libusb_init(NULL) < 0) {
    if(!usb_quiet) {
      fprintf(stderr, "error: could not initialize libusb-1.0: %s\n",
              libusb_strerror(result));
    }
    goto done;
  }

  handle = usb_open(NULL, info);
  result = handle != NULL;

 done:
  if(handle != NULL) {
    libusb_close(handle);
  }
  libusb_exit(NULL);

  return result;
}

//-----------------------------------------------------------------------------

int usb_control(DeviceInfo *info, uint8_t message) {
  return usb_message(info, LIBUSB_ENDPOINT_OUT, message, 0, 0, NULL, 0);
}

//-----------------------------------------------------------------------------

int usb_send(DeviceInfo *info, uint8_t message, uint16_t value, uint16_t index, uint8_t* buf, uint16_t size) {
  return usb_message(info, LIBUSB_ENDPOINT_OUT, message, value, 0, buf, size);
}

//-----------------------------------------------------------------------------

int usb_receive(DeviceInfo* info, uint8_t message, uint16_t value, uint16_t index, uint8_t* buf, uint16_t size) {
  return usb_message(info, LIBUSB_ENDPOINT_IN, message, value, index, buf, size);
}

//-----------------------------------------------------------------------------

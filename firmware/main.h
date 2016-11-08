#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include "usbdrv/usbdrv.h"

#define ATTR_NO_INIT __attribute__ ((section(".noinit")))
#define ATTR_INIT_SECTION_3 __attribute__ ((used, naked, section(".init3")))

#define MAGIC 0xDEADBEEF
#define BOOTLOADER (0x20000 - 0x2000)

#define LOCK_NONE   0x00
#define LOCK_SERIAL 0x01
#define LOCK_MAP    0x02

#define SERIAL_COMMAND_NONE      0x00
#define SERIAL_COMMAND_EXECUTE   0x01
#define SERIAL_COMMAND_KEY_DOWN  0x02
#define SERIAL_COMMAND_KEY_UP    0x03
#define SERIAL_COMMAND_KEY_PRESS 0x04
#define SERIAL_COMMAND_MAP       0x05

uint8_t SERIAL_COMMAND_ARITY_FOR[6] = { 0, 1, 1, 1, 1, 2 };

typedef struct {
  uint8_t bit;
  uint8_t byte;
  uint8_t command;
  uint8_t arguments[2];
  uint8_t index;
} Serial;

typedef struct {
  uint8_t volatile *pins;
  uint8_t mask;
  uint8_t key;
} Mapping;

void SetupHardware(void);
void SetupSerial(void);
void ResetSerial(void);
void ExpectNextSerialByte(void);
void ExecuteSerialCommand(void);
void SetupMappings(void);
void Map(uint8_t port, uint8_t mask, uint8_t key);
void ApplyMappings(void);
void SetupKeyboardLayout(void);
void ExecuteImmediateCommands(volatile Config *cfg, uint16_t delay);
void DisableJTAG(void);
void ResetCounter(void);
void ClockMatrix(void);
bool ScanMatrix(void);
void RelayMatrix(void);
void RelayKeyPress(volatile uint8_t key);
bool IsKeyDown(volatile uint8_t key);
bool IsKeyUp(volatile uint8_t key);
bool IsKey(volatile uint8_t key);
bool QueryKeyDown(volatile uint8_t key);
bool QueryKeyUp(volatile uint8_t key);
void ExecuteKey(uint8_t key);
void ExecuteBindingInteractively(Binding *binding);
void ExecuteBinding(Binding *binding);
void ExecuteCommand(volatile Config *config, Command* command);
void ResetCrosspointSwitch22106(void);
void StrobeCrosspointSwitch22106(void);
void ResetCrosspointSwitch8808(void);
void StrobeCrosspointSwitch8808(void);
void SetCrosspointSwitch(uint8_t index, bool closed);
void SetCrosspointSwitchLocked(uint8_t index, bool closed, uint8_t lock);
void Type(char *string);
void SaveState(void);
void RestoreState(void);
void EnterBootloader(void);
void CheckBootloader(void) ATTR_INIT_SECTION_3;
int ReadEeprom(FILE* file);

void SetupUSB(void);
USB_PUBLIC usbMsgLen_t usbFunctionSetup(uint8_t data[8]);
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len);
void ExecuteCommandsFromUSBData(void);
#endif // MAIN_H

#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include "usbdrv/usbdrv.h"

#define ATTR_NO_INIT __attribute__ ((section(".noinit")))
#define ATTR_INIT_SECTION_3 __attribute__ ((used, naked, section(".init3")))

#define MAGIC 0xDEADBEEF
#define BOOTLOADER (0x20000 - 0x2000)

uint8_t KEY_ARROWLEFT = 15;

uint8_t SERIAL_COMMAND  = 0x10;
uint8_t SERIAL_ARGUMENT = 0x00;

#define SERIAL_COMMAND_EXECUTE   0x01
#define SERIAL_COMMAND_KEY_DOWN  0x02
#define SERIAL_COMMAND_KEY_UP    0x03
#define SERIAL_COMMAND_KEY_PRESS 0x04

typedef struct {
  uint8_t bit;
  uint8_t data;
  uint8_t command;
  uint8_t argument;
  uint8_t expected;
} Serial;

void SetupHardware(void);
void SetupSerial(void);
void ResetSerial(void);
void ExpectSerialCommand(void);
void ExpectSerialArgument(void);
void ExecuteSerialCommand(uint8_t command, uint8_t argument);
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
void ExecuteBinding(Binding *binding);
void ExecuteCommand(volatile Config *config, Command* command);
void ResetCrosspointSwitch22106(void);
void StrobeCrosspointSwitch22106(void);
void ResetCrosspointSwitch8808(void);
void StrobeCrosspointSwitch8808(void);
void SetCrosspointSwitch(uint8_t index, bool closed);
void SetCrosspointSwitchLocked(uint8_t index, bool closed);
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

#ifndef MAIN_H
#define MAIN_H

#include "config.h"

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
void ApplyConfig(void);
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
void ExecuteBinding(uint8_t key);
void ExecuteCommand(Command* command);
void ResetCrosspointSwitch(void);
void SetCrosspointSwitch(uint8_t index, bool closed);
void SetCrosspointSwitchLocked(uint8_t index, bool closed);
void Type(char *string);
void EnterBootloader(void);
void CheckBootloader(void) ATTR_INIT_SECTION_3;
int ReadEeprom(FILE* file);

#endif // MAIN_H

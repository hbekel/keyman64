#ifndef MAIN_H
#define MAIN_H

#define xstr(s) mstr(s)
#define mstr(s) #s

#include "config.h"
#include "usbdrv/usbdrv.h"

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

typedef struct {
  uint32_t magic;
  State* state;
  char password[64];
  uint16_t bootflag;
} Storage;

uint16_t BOOTLOADER_MAGIC = 0xb0b0;
uint32_t STORAGE_MAGIC = 0xABCD1234;
uint16_t STORAGE_ADDRESS = 0x1000 - sizeof(Storage);

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
uint8_t ReadKey(void);
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
void Type(char* fmt, ...);
void Typestr(char *string);
void SetupVersionString(void);
void ShowState(void);
void GetState(State* state);
void SetState(State* state);
void SaveState(void);
void RestoreState(void);
void EnterBootloader(void);
void Reset(void);
int ReadEeprom(FILE* file);

void SetupUSB(void);
USB_PUBLIC usbMsgLen_t usbFunctionSetup(uint8_t data[8]);
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len);
void FlashConfigurationFromUSBData(void);
void ExecuteCommandsFromUSBData(void);

void SetPassword(void);
void EnterPassword(const char* prompt, char* buffer);
void SavePassword(char* buffer);
void LoadPassword(void);

Storage* Storage_new(void);
bool Storage_valid(Storage* self);
void Storage_bootstrap(Storage* self);

void Storage_load(Storage* self);
void Storage_load_magic(Storage* self);
void Storage_load_state(Storage* self);
void Storage_load_password(Storage* self);
void Storage_load_bootflag(Storage* self);

void Storage_save(Storage* self);
void Storage_save_magic(Storage* self);
void Storage_save_state(Storage* self);
void Storage_save_password(Storage* self);
void Storage_save_bootflag(Storage* self);
void Storage_free(Storage* self);

#endif // MAIN_H

#ifndef MAIN_H
#define MAIN_H

#include "config.h"

key_t KEY_INIT = { .col = 15, .row = 14 };
key_t KEY_META = { .col = 7,  .row = 1 };
  
static void SetupHardware(void);
static void ApplyConfig(void);
static void DisableJTAG(void);
static void ResetCounter(void);
static void ClockMatrix(void);
static bool ScanMatrix(void);
static void RelayMatrix(void);
static bool KeyEquals(key_t key, key_t other);
static void RelayKeyPress(key_t key);
static bool QueryKeyPress(key_t key);
static void ReadKeyPress(key_t* key);
static void ExecuteCommand(command_t* command);
static void ResetCrosspointSwitch(void);
static void SetCrosspointSwitch(uint8_t index, bool closed);

#endif // MAIN_H

#ifndef MAIN_H
#define MAIN_H

#include "config.h"

Key KEY_META = { .col = 7,  .row = 1 };
  
static void SetupHardware(void);
static void ApplyConfig(void);
static void DisableJTAG(void);
static void ResetCounter(void);
static void ClockMatrix(void);
static bool ScanMatrix(void);
static void RelayMatrix(void);
static void RelayKeyPress(Key key);
static bool QueryKeyPress(Key key);
static void ReadKeyPress(Key* key);
static void ExecuteBinding(Key* key);
static void ExecuteCommand(Command* command);
static void ResetCrosspointSwitch(void);
static void SetCrosspointSwitch(uint8_t index, bool closed);

#endif // MAIN_H

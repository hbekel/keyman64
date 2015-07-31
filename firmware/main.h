#ifndef MAIN_H
#define MAIN_H

#include "config.h"

Key KEY_META = { .col = 7,  .row = 1 };
  
void SetupHardware(void);
void ApplyConfig(void);
void DisableJTAG(void);
void ResetCounter(void);
void ClockMatrix(void);
bool ScanMatrix(void);
void RelayMatrix(void);
void RelayKeyPress(Key key);
bool QueryKeyPress(Key key);
void ReadKeyPress(Key* key);
void ExecuteBinding(Key* key);
void ExecuteCommand(Command* command);
void ResetCrosspointSwitch(void);
void SetCrosspointSwitch(uint8_t index, bool closed);

int ReadEeprom(FILE* file);
FILE eeprom = FDEV_SETUP_STREAM(NULL, ReadEeprom, _FDEV_SETUP_READ);

#endif // MAIN_H

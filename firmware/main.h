#ifndef MAIN_H
#define MAIN_H

#include "config.h"

Key KEY_ARROWLEFT = { .col = 7,  .row = 1 };
  
void SetupHardware(void);
void SetupSerial(void);
void ApplyConfig(void);
void DisableJTAG(void);
void ResetCounter(void);
void ClockMatrix(void);
bool ScanMatrix(void);
void RelayMatrix(void);
void RelayKeyPress(volatile Key *key);
bool IsKeyDown(volatile Key *key);
bool IsKeyUp(volatile Key *key);
bool IsKey(volatile Key *key);
bool QueryKeyDown(volatile Key *key);
bool QueryKeyUp(volatile Key *key);
void ExecuteBinding(Key* key);
void ExecuteCommand(Command* command);
void ResetCrosspointSwitch(void);
void SetCrosspointSwitch(uint8_t index, bool closed);
int ReadEeprom(FILE* file);

#endif // MAIN_H

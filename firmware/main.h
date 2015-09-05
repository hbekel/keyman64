#ifndef MAIN_H
#define MAIN_H

#include "config.h"

uint8_t KEY_ARROWLEFT = 15;
  
void SetupHardware(void);
void SetupSerial(void);
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
int ReadEeprom(FILE* file);

#endif // MAIN_H

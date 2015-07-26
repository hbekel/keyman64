#ifndef MAIN_H
#define MAIN_H

typedef struct {
  uint8_t col;
  uint8_t row;
} key_t;

key_t KEY_ARROW_LEFT = { .col = 7, .row = 1 };
key_t KEY_K          = { .col = 4, .row = 5 }; 
key_t KEY_3          = { .col = 1, .row = 0 };

static void SetupHardware(void);
static void DisableJTAG(void);
static void ResetCounter(void);
static void ClockMatrix(void);
static bool ScanMatrix(void);
static void RelayMatrix(void);
static bool KeysEqual(key_t key, key_t other);
static void RelayKeyPress(key_t key);
static bool QueryKeyPress(key_t key);
static void ReadKeyPress(key_t* key);

#endif // MAIN_H

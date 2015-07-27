#ifndef MAIN_H
#define MAIN_H

typedef struct {
  uint8_t col;
  uint8_t row;
} key_t;

key_t KEY_META = { .col = 7, .row = 1 };
key_t KEY_1    = { .col = 7, .row = 0 };
key_t KEY_2    = { .col = 7, .row = 3 };
key_t KEY_3    = { .col = 1, .row = 0 };
key_t KEY_4    = { .col = 1, .row = 3 };
key_t KEY_5    = { .col = 2, .row = 0 };

typedef enum {ACTION_SET, ACTION_INV, ACTION_INC, ACTION_DEC, ACTION_TRS} action_t;
typedef enum {PORT_A, PORT_B} port_t;

typedef struct {
  action_t action;
  port_t port;
  uint8_t mask;
  uint8_t data;
} command_t;

static void SetupHardware(void);
static void DisableJTAG(void);
static void ResetCounter(void);
static void ClockMatrix(void);
static bool ScanMatrix(void);
static void RelayMatrix(void);
static bool KeyEquals(key_t key, key_t other);
static void RelayKeyPress(key_t key);
static bool QueryKeyPress(key_t key);
static void ReadKeyPress(key_t* key);
static void ExecuteCommand(command_t command);
#endif // MAIN_H

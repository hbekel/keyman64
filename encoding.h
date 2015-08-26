#ifndef ENCODING_H
#define ENCODING_H

#include <stdint.h>

#define DOWN(n)  (n | 0x80)
#define UP(n)    (n | 0x40)
#define PRESS(n) (n | 0xc0)
#define SHIFT(n) DOWN(57), PRESS(n), UP(57)

#define DEFINE(c, s) { .code = (uint8_t[]) { c }, .size = s }
#define UNDEFINED { .code = (uint8_t[]) {}, .size = 0 }

typedef struct {
  uint8_t *code;
  uint8_t size;
} Sequence;

Sequence encoding[127] = {

  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  DEFINE(PRESS(8), 1),
  UNDEFINED,
  UNDEFINED,

  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,

  DEFINE(PRESS(39), 1), // SPACE 
  DEFINE(SHIFT(7),  3), // !
  DEFINE(SHIFT(31), 3), // "
  DEFINE(SHIFT(1),  3), // #
  DEFINE(SHIFT(25), 3), // $  
};

#undef DOWN
#undef UP
#undef PRESS
#undef SHIFT
#undef DEFINE
#undef UNDEFINED

#endif

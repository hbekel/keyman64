#ifndef ENCODING_H
#define ENCODING_H

#include <stdint.h>

#define CODE_KEY_DOWN  0x80U
#define CODE_KEY_UP    0x40U
#define CODE_KEY_PRESS 0xC0U
#define CODE_MASK      0xC0U

#define DOWN(n)  ((uint8_t)n | CODE_KEY_DOWN)
#define UP(n)    ((uint8_t)n | CODE_KEY_UP)
#define PRESS(n) ((uint8_t)n | CODE_KEY_PRESS)
#define SHIFT(n) DOWN(57), PRESS((uint8_t)n), UP(57)

#define DEFINE(c, s) { .codes = (uint8_t[]) { c }, .size = s }
#define UNDEFINED { .codes = (uint8_t[]) {}, .size = 0 }

typedef struct {
  uint8_t *codes;
  uint8_t size;
} Sequence;

Sequence encoding[128] = {

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
  DEFINE(PRESS(8), 1), // CR
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
  DEFINE(SHIFT(2),  3), // %
  DEFINE(SHIFT(26), 3), // &
  DEFINE(SHIFT(3),  3), // '
  DEFINE(SHIFT(27), 3), // (
  DEFINE(SHIFT(4),  3), // )
  DEFINE(PRESS(14), 1), // *
  DEFINE(PRESS(5),  1), // +
  DEFINE(PRESS(61), 1), // ,
  DEFINE(PRESS(29), 1), // -
  DEFINE(PRESS(37), 1), // .
  DEFINE(PRESS(62), 1), // /
  DEFINE(PRESS(28), 1), // 0
  DEFINE(PRESS(7),  1), // 1
  DEFINE(PRESS(31), 1), // 2
  DEFINE(PRESS(1),  1), // 3
  DEFINE(PRESS(25), 1), // 4
  DEFINE(PRESS(2),  1), // 5
  DEFINE(PRESS(26), 1), // 6
  DEFINE(PRESS(3),  1), // 7
  DEFINE(PRESS(27), 1), // 8
  DEFINE(PRESS(4),  1), // 9
  DEFINE(PRESS(45), 1), // :
  DEFINE(PRESS(22), 1), // ;
  DEFINE(SHIFT(61), 3), // <
  DEFINE(PRESS(46), 1), // =
  DEFINE(SHIFT(37), 3), // <
  DEFINE(SHIFT(62), 3), // ?
  DEFINE(PRESS(53), 1), // @
  DEFINE(SHIFT(17), 3), // A
  DEFINE(SHIFT(35), 3), // B
  DEFINE(SHIFT(34), 3), // C
  DEFINE(SHIFT(18), 3), // D
  DEFINE(SHIFT(49), 3), // E
  DEFINE(SHIFT(42), 3), // F
  DEFINE(SHIFT(19), 3), // G
  DEFINE(SHIFT(43), 3), // H
  DEFINE(SHIFT(12), 3), // I
  DEFINE(SHIFT(20), 3), // J
  DEFINE(SHIFT(44), 3), // K
  DEFINE(SHIFT(21), 3), // L
  DEFINE(SHIFT(36), 3), // M
  DEFINE(SHIFT(60), 3), // N
  DEFINE(SHIFT(52), 3), // O
  DEFINE(SHIFT(13), 3), // P
  DEFINE(SHIFT(55), 3), // Q
  DEFINE(SHIFT(10), 3), // R
  DEFINE(SHIFT(41), 3), // S
  DEFINE(SHIFT(50), 3), // T
  DEFINE(SHIFT(51), 3), // U
  DEFINE(SHIFT(59), 3), // V
  DEFINE(SHIFT(9),  3), // W
  DEFINE(SHIFT(58), 3), // X
  DEFINE(SHIFT(11), 3), // Y
  DEFINE(SHIFT(33), 3), // Z 
  DEFINE(SHIFT(45), 3), // [
  DEFINE(PRESS(6),  1), // \ (POUND)
  DEFINE(SHIFT(22), 3), // ]
  DEFINE(PRESS(54), 1), // ^ (ARROWUP)
  DEFINE(PRESS(15), 1), // _ (ARROWLEFT)
  DEFINE(SHIFT(8),  3), // ` (SHIFT-RETURN)
  DEFINE(PRESS(17), 1), // a
  DEFINE(PRESS(35), 1), // b
  DEFINE(PRESS(34), 1), // c
  DEFINE(PRESS(18), 1), // d
  DEFINE(PRESS(49), 1), // e
  DEFINE(PRESS(42), 1), // f
  DEFINE(PRESS(19), 1), // g
  DEFINE(PRESS(43), 1), // h
  DEFINE(PRESS(12), 1), // i
  DEFINE(PRESS(20), 1), // j
  DEFINE(PRESS(44), 1), // k
  DEFINE(PRESS(21), 1), // l
  DEFINE(PRESS(36), 1), // m
  DEFINE(PRESS(60), 1), // n
  DEFINE(PRESS(52), 1), // o
  DEFINE(PRESS(13), 1), // p
  DEFINE(PRESS(55), 1), // q
  DEFINE(PRESS(10), 1), // r
  DEFINE(PRESS(41), 1), // s
  DEFINE(PRESS(50), 1), // t
  DEFINE(PRESS(51), 1), // u
  DEFINE(PRESS(59), 1), // v
  DEFINE(PRESS(9),  1), // w
  DEFINE(PRESS(58), 1), // x
  DEFINE(PRESS(11), 1), // y
  DEFINE(PRESS(33), 1), // z
  UNDEFINED,            // {
  UNDEFINED,            // |
  UNDEFINED,            // }
  DEFINE(PRESS(8),  1), // ~ (RETURN)
  DEFINE(PRESS(0),  1), // DEL
};

#undef DOWN
#undef UP
#undef PRESS
#undef SHIFT
#undef DEFINE
#undef UNDEFINED

#endif

#ifndef ENCODING_H
#define ENCODING_H

#include <stdint.h>
#include "../keyboard.h"

#define CODE_KEY_DOWN  0x80U
#define CODE_KEY_UP    0x40U
#define CODE_KEY_PRESS 0xC0U
#define CODE_MASK      0xC0U

#define DOWN(n)  ((uint8_t)n | CODE_KEY_DOWN)
#define UP(n)    ((uint8_t)n | CODE_KEY_UP)
#define PRESS(n) ((uint8_t)n | CODE_KEY_PRESS)
#define SHIFT(n) DOWN(KEY_SHIFTLEFT), PRESS((uint8_t)n), UP(KEY_SHIFTLEFT)
#define CTRL(n)  DOWN(KEY_CONTROL), PRESS((uint8_t)n), UP(KEY_CONTROL)
#define CBM(n)   DOWN(KEY_CBM), PRESS((uint8_t)n), UP(KEY_CBM)
#define DEFINE(c, s) { .codes = (uint8_t[]) { c }, .size = s }
#define UNDEFINED { .codes = (uint8_t[]) {}, .size = 0 }

typedef struct {
  uint8_t *codes;
  uint8_t size;
} Sequence;

Sequence encoding[256] = {
  UNDEFINED, 
  UNDEFINED, 
  UNDEFINED,
  DEFINE(PRESS(KEY_RUNSTOP),   1),  // RUNSTOP  
  UNDEFINED, 
  DEFINE(CTRL(KEY_2),          3),  // WHITE
  UNDEFINED, 
  UNDEFINED, 
  UNDEFINED,                        // LOCK UPPER/LOWERCASE (not typeable)
  UNDEFINED,                        // UNLOCK UPPER/LOWERCASE (not typeable)
  DEFINE(SHIFT(KEY_RETURN),    3),  // LINE FEED (ASCII only) \n
  UNDEFINED,
  DEFINE(SHIFT(KEY_CLRHOME),   3),  // FORM FEED (ASCII only) \f
  DEFINE(PRESS(KEY_RETURN),    1),  // CARRIAGE RETURN \n
  UNDEFINED,                        // SWITCH TO UPPERCASE (not typeable)
  UNDEFINED,
  UNDEFINED,
  DEFINE(PRESS(KEY_CURSORDOWN), 1), // CURSOR DOWN
  DEFINE(CTRL(KEY_9),           3), // RVS ON
  DEFINE(PRESS(KEY_CLRHOME),    1), // HOME
  DEFINE(PRESS(KEY_INSTDEL),    1), // DEL
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  DEFINE(CTRL(KEY_3),           3), // RED 
  DEFINE(PRESS(KEY_CURSORLEFT), 1), // CURSOR LEFT
  DEFINE(CTRL(KEY_6),           3), // GREEN
  DEFINE(CTRL(KEY_7),           3), // BLUE
  DEFINE(PRESS(KEY_SPACE),      1), // SPACE 
  DEFINE(SHIFT(KEY_1),          3), // !
  DEFINE(SHIFT(KEY_2),          3), // "
  DEFINE(SHIFT(KEY_3),          3), // #
  DEFINE(SHIFT(KEY_4),          3), // $  
  DEFINE(SHIFT(KEY_5),          3), // %
  DEFINE(SHIFT(KEY_6),          3), // &
  DEFINE(SHIFT(KEY_7),          3), // '
  DEFINE(SHIFT(KEY_8),          3), // (
  DEFINE(SHIFT(KEY_9),          3), // )
  DEFINE(PRESS(KEY_ASTERISK),   1), // *
  DEFINE(PRESS(KEY_PLUS),       1), // +
  DEFINE(PRESS(KEY_COMMA),      1), // ,
  DEFINE(PRESS(KEY_MINUS),      1), // -
  DEFINE(PRESS(KEY_PERIOD),     1), // .
  DEFINE(PRESS(KEY_SLASH),      1), // /
  DEFINE(PRESS(KEY_0),          1), // 0
  DEFINE(PRESS(KEY_1),          1), // 1
  DEFINE(PRESS(KEY_2),          1), // 2
  DEFINE(PRESS(KEY_3),          1), // 3
  DEFINE(PRESS(KEY_4),          1), // 4
  DEFINE(PRESS(KEY_5),          1), // 5
  DEFINE(PRESS(KEY_6),          1), // 6
  DEFINE(PRESS(KEY_7),          1), // 7
  DEFINE(PRESS(KEY_8),          1), // 8
  DEFINE(PRESS(KEY_9),          1), // 9
  DEFINE(PRESS(KEY_COLON),      1), // :
  DEFINE(PRESS(KEY_SEMICOLON),  1), // ;
  DEFINE(SHIFT(KEY_COMMA),      3), // <
  DEFINE(PRESS(KEY_EQUAL),      1), // =
  DEFINE(SHIFT(KEY_PERIOD),     3), // <
  DEFINE(SHIFT(KEY_SLASH),      3), // ?
  DEFINE(PRESS(KEY_AT),         1), // @
  DEFINE(SHIFT(KEY_A),          3), // A
  DEFINE(SHIFT(KEY_B),          3), // B
  DEFINE(SHIFT(KEY_C),          3), // C
  DEFINE(SHIFT(KEY_D),          3), // D
  DEFINE(SHIFT(KEY_E),          3), // E
  DEFINE(SHIFT(KEY_F),          3), // F
  DEFINE(SHIFT(KEY_G),          3), // G
  DEFINE(SHIFT(KEY_H),          3), // H
  DEFINE(SHIFT(KEY_I),          3), // I
  DEFINE(SHIFT(KEY_J),          3), // J
  DEFINE(SHIFT(KEY_K),          3), // K
  DEFINE(SHIFT(KEY_L),          3), // L
  DEFINE(SHIFT(KEY_M),          3), // M
  DEFINE(SHIFT(KEY_N),          3), // N
  DEFINE(SHIFT(KEY_O),          3), // O
  DEFINE(SHIFT(KEY_P),          3), // P
  DEFINE(SHIFT(KEY_Q),          3), // Q
  DEFINE(SHIFT(KEY_R),          3), // R
  DEFINE(SHIFT(KEY_S),          3), // S
  DEFINE(SHIFT(KEY_T),          3), // T
  DEFINE(SHIFT(KEY_U),          3), // U
  DEFINE(SHIFT(KEY_V),          3), // V
  DEFINE(SHIFT(KEY_W),          3), // W
  DEFINE(SHIFT(KEY_X),          3), // X
  DEFINE(SHIFT(KEY_Y),          3), // Y
  DEFINE(SHIFT(KEY_Z),          3), // Z 
  DEFINE(SHIFT(KEY_COLON),      3), // [
  DEFINE(PRESS(KEY_POUND),      1), // \ (POUND)
  DEFINE(SHIFT(KEY_SEMICOLON),  3), // ]
  DEFINE(PRESS(KEY_UPARROW),    1), // ^ (UPARROW)
  DEFINE(PRESS(KEY_BACKARROW),  1), // _ (BACKARROW)
  DEFINE(SHIFT(KEY_ASTERISK),   3), // ` (CENTERED THICK BAR)                @ 96
  DEFINE(PRESS(KEY_A),          1), // a
  DEFINE(PRESS(KEY_B),          1), // b
  DEFINE(PRESS(KEY_C),          1), // c
  DEFINE(PRESS(KEY_D),          1), // d
  DEFINE(PRESS(KEY_E),          1), // e
  DEFINE(PRESS(KEY_F),          1), // f
  DEFINE(PRESS(KEY_G),          1), // g
  DEFINE(PRESS(KEY_H),          1), // h
  DEFINE(PRESS(KEY_I),          1), // i
  DEFINE(PRESS(KEY_J),          1), // j
  DEFINE(PRESS(KEY_K),          1), // k
  DEFINE(PRESS(KEY_L),          1), // l
  DEFINE(PRESS(KEY_M),          1), // m
  DEFINE(PRESS(KEY_N),          1), // n
  DEFINE(PRESS(KEY_O),          1), // o
  DEFINE(PRESS(KEY_P),          1), // p
  DEFINE(PRESS(KEY_Q),          1), // q
  DEFINE(PRESS(KEY_R),          1), // r
  DEFINE(PRESS(KEY_S),          1), // s
  DEFINE(PRESS(KEY_T),          1), // t
  DEFINE(PRESS(KEY_U),          1), // u
  DEFINE(PRESS(KEY_V),          1), // v
  DEFINE(PRESS(KEY_W),          1), // w
  DEFINE(PRESS(KEY_X),          1), // x
  DEFINE(PRESS(KEY_Y),          1), // y
  DEFINE(PRESS(KEY_Z),          1), // z
  DEFINE(SHIFT(KEY_PLUS),       3), // { (CENTERED ORTHOGONAL THICK CROSS)
  DEFINE(CBM(KEY_MINUS),        3), // | (LEFT HALF CHECKERBOARD)
  DEFINE(SHIFT(KEY_B),          3), // } (MIDDLE VERTICAL THICK LINE)          
  DEFINE(CBM(KEY_UPARROW),      3), // ~ (PI)
  DEFINE(CBM(KEY_ASTERISK),     3), // DEL (TRIANGLE TOP RIGHT)              @ 127
  UNDEFINED,
  DEFINE(CBM(KEY_1),            3), // ORANGE
  UNDEFINED,
  UNDEFINED,
  UNDEFINED,
  DEFINE(PRESS(KEY_F1),         1), // F1
  DEFINE(PRESS(KEY_F3),         1), // F3
  DEFINE(PRESS(KEY_F5),         1), // F5
  DEFINE(PRESS(KEY_F7),         1), // F7
  DEFINE(SHIFT(KEY_F1),         3), // F2
  DEFINE(SHIFT(KEY_F3),         3), // F4
  DEFINE(SHIFT(KEY_F5),         3), // F6
  DEFINE(SHIFT(KEY_F7),         3), // F8
  DEFINE(SHIFT(KEY_RETURN),     3), // MOVE TO BEGINNING OF NEXT LINE
  UNDEFINED,                        // SWITCH TO UPPERCASE (not typeable)
  UNDEFINED,
  DEFINE(CTRL(KEY_1),           3), // BLACK
  DEFINE(SHIFT(KEY_CURSORDOWN), 3), // CURSOR DOWN 
  DEFINE(CTRL(KEY_0),           3), // RVS OFF
  DEFINE(SHIFT(KEY_CLRHOME),    3), // CLEAR SCREEN & HOME CURSOR
  DEFINE(PRESS(KEY_INSTDEL),    1), // DEL
  DEFINE(CBM(KEY_2),            3), // BROWN
  DEFINE(CBM(KEY_3),            3), // LIGHTRED
  DEFINE(CBM(KEY_4),            3), // DARKGREY
  DEFINE(CBM(KEY_5),            3), // GREY
  DEFINE(CBM(KEY_6),            3), // LIGHTGREEN
  DEFINE(CBM(KEY_7),            3), // LIGHTBLUE
  DEFINE(CBM(KEY_8),            3), // LIGHTGREY
  DEFINE(CTRL(KEY_5),           3), // PURPLE
  DEFINE(SHIFT(KEY_CURSORLEFT), 3), // CURSOR RIGHT
  DEFINE(CTRL(KEY_8),           3), // YELLOW
  DEFINE(CTRL(KEY_4),           3), // CYAN
  DEFINE(PRESS(KEY_SPACE),      1), // SPACE                                @ 160
  DEFINE(CBM(KEY_K),            3),
  DEFINE(CBM(KEY_I),            3),
  DEFINE(CBM(KEY_T),            3),
  DEFINE(CBM(KEY_AT),           3),
  DEFINE(CBM(KEY_G),            3),
  DEFINE(CBM(KEY_PLUS),         3),
  DEFINE(CBM(KEY_M),            3),
  DEFINE(CBM(KEY_POUND),        3),
  DEFINE(SHIFT(KEY_POUND),      3),
  DEFINE(CBM(KEY_L),            3),
  DEFINE(CBM(KEY_Q),            3),
  DEFINE(CBM(KEY_D),            3),
  DEFINE(CBM(KEY_Z),            3),
  DEFINE(CBM(KEY_S),            3),
  DEFINE(CBM(KEY_P),            3),
  DEFINE(CBM(KEY_A),            3),
  DEFINE(CBM(KEY_E),            3),
  DEFINE(CBM(KEY_R),            3),
  DEFINE(CBM(KEY_W),            3),
  DEFINE(CBM(KEY_H),            3),
  DEFINE(CBM(KEY_J),            3),
  DEFINE(CBM(KEY_L),            3),
  DEFINE(CBM(KEY_Y),            3),
  DEFINE(CBM(KEY_U),            3),
  DEFINE(CBM(KEY_O),            3),
  DEFINE(SHIFT(KEY_AT),         3),
  DEFINE(CBM(KEY_F),            3),
  DEFINE(CBM(KEY_C),            3),
  DEFINE(CBM(KEY_X),            3),
  DEFINE(CBM(KEY_C),            3), //                                      @ 190
  DEFINE(CBM(KEY_B),            3),

  // codes 192-223 equal 96-127

  DEFINE(SHIFT(KEY_ASTERISK),   3), // ` (CENTERED THICK BAR)               @ 129
  DEFINE(PRESS(KEY_A),          1), // a
  DEFINE(PRESS(KEY_B),          1), // b
  DEFINE(PRESS(KEY_C),          1), // c
  DEFINE(PRESS(KEY_D),          1), // d
  DEFINE(PRESS(KEY_E),          1), // e
  DEFINE(PRESS(KEY_F),          1), // f
  DEFINE(PRESS(KEY_G),          1), // g
  DEFINE(PRESS(KEY_H),          1), // h
  DEFINE(PRESS(KEY_I),          1), // i
  DEFINE(PRESS(KEY_J),          1), // j
  DEFINE(PRESS(KEY_K),          1), // k
  DEFINE(PRESS(KEY_L),          1), // l
  DEFINE(PRESS(KEY_M),          1), // m
  DEFINE(PRESS(KEY_N),          1), // n
  DEFINE(PRESS(KEY_O),          1), // o
  DEFINE(PRESS(KEY_P),          1), // p
  DEFINE(PRESS(KEY_Q),          1), // q
  DEFINE(PRESS(KEY_R),          1), // r
  DEFINE(PRESS(KEY_S),          1), // s
  DEFINE(PRESS(KEY_T),          1), // t
  DEFINE(PRESS(KEY_U),          1), // u
  DEFINE(PRESS(KEY_V),          1), // v
  DEFINE(PRESS(KEY_W),          1), // w
  DEFINE(PRESS(KEY_X),          1), // x
  DEFINE(PRESS(KEY_Y),          1), // y
  DEFINE(PRESS(KEY_Z),          1), // z
  DEFINE(SHIFT(KEY_PLUS),       3), // { (CENTERED ORTHOGONAL THICK CROSS)
  DEFINE(CBM(KEY_MINUS),        3), // | (LEFT HALF CHECKERBOARD)
  DEFINE(SHIFT(KEY_B),          3), // } (MIDDLE VERTICAL THICK LINE)          
  DEFINE(CBM(KEY_UPARROW),      3), // ~ (PI, CBM-ARROWUP)
  DEFINE(CBM(KEY_ASTERISK),     3), // DEL (TRIANGLE TOP RIGHT)             @ 223

  // codes 224-254 equal 160-190

  DEFINE(PRESS(KEY_SPACE),      1), // SPACE                                @ 224
  DEFINE(CBM(KEY_K),            3),
  DEFINE(CBM(KEY_I),            3),
  DEFINE(CBM(KEY_T),            3),
  DEFINE(CBM(KEY_AT),           3),
  DEFINE(CBM(KEY_G),            3),
  DEFINE(CBM(KEY_PLUS),         3),
  DEFINE(CBM(KEY_M),            3),
  DEFINE(CBM(KEY_POUND),        3),
  DEFINE(SHIFT(KEY_POUND),      3),
  DEFINE(CBM(KEY_L),            3),
  DEFINE(CBM(KEY_Q),            3),
  DEFINE(CBM(KEY_D),            3),
  DEFINE(CBM(KEY_Z),            3),
  DEFINE(CBM(KEY_S),            3),
  DEFINE(CBM(KEY_P),            3),
  DEFINE(CBM(KEY_A),            3),
  DEFINE(CBM(KEY_E),            3),
  DEFINE(CBM(KEY_R),            3),
  DEFINE(CBM(KEY_W),            3),
  DEFINE(CBM(KEY_H),            3),
  DEFINE(CBM(KEY_J),            3),
  DEFINE(CBM(KEY_L),            3),
  DEFINE(CBM(KEY_Y),            3),
  DEFINE(CBM(KEY_U),            3),
  DEFINE(CBM(KEY_O),            3),
  DEFINE(SHIFT(KEY_AT),         3),
  DEFINE(CBM(KEY_F),            3),
  DEFINE(CBM(KEY_C),            3),
  DEFINE(CBM(KEY_X),            3),
  DEFINE(CBM(KEY_C),            3),  //                                      @ 254

  // code 255 equals 126

  DEFINE(CBM(KEY_UPARROW),      3), // ~ (PI, CBM-ARROWUP)
  
};

#undef DOWN
#undef UP
#undef PRESS
#undef SHIFT
#undef CTRL
#undef CBM
#undef DEFINE
#undef UNDEFINED

#endif

#ifndef KEYS_H
#define KEYS_H

#include "config.h"

typedef struct {
  char name[32];
  uint8_t key;
} Symbol;

Symbol symbols[64+25] = {
  
{ .name = "DEL",         0 },
{ .name = "THREE",       1 },
{ .name = "FIVE",        2 },
{ .name = "SEVEN",       3 },
{ .name = "NINE",        4 },
{ .name = "+",           5 },
{ .name = "POUND",       6 },
{ .name = "ONE",         7 },

{ .name = "RETURN",      8 },
{ .name = "W",           9 },
{ .name = "R",           10 },
{ .name = "Y",           11 },
{ .name = "I",           12 },
{ .name = "P",           13 },
{ .name = "*",           14 },
{ .name = "ARROWLEFT",   15 },

{ .name = "CURSORRIGHT", 16 },
{ .name = "A",           17 },
{ .name = "D",           18 },
{ .name = "G",           19 },
{ .name = "J",           20 },
{ .name = "L",           21 },
{ .name = ";",           22 },
{ .name = "CONTROL",     23 },

{ .name = "F7",          24 },
{ .name = "FOUR",        25 },
{ .name = "SIX",         26 },
{ .name = "EIGHT",       27 },
{ .name = "ZERO",        28 },
{ .name = "-",           29 },
{ .name = "HOME",        30 },
{ .name = "TWO",         31 },

{ .name = "F1",          32 },
{ .name = "Z",           33 },
{ .name = "C",           34 },
{ .name = "B",           35 },
{ .name = "M",           36 },
{ .name = ".",           37 },
{ .name = "RIGHTSHIFT",  38 },
{ .name = "SPACE",       39 },

{ .name = "F3",          40 },
{ .name = "S",           41 },
{ .name = "F",           42 },
{ .name = "H",           43 },
{ .name = "K",           44 },
{ .name = "COLON",       45 },
{ .name = "=",           46 },
{ .name = "COMMODORE",   47 },

{ .name = "F5",          48 },
{ .name = "E",           49 },
{ .name = "T",           50 },
{ .name = "U",           51 },
{ .name = "O",           52 },
{ .name = "@",           53 },
{ .name = "ARROWUP",     54 },
{ .name = "Q",           55 },

{ .name = "CURSORDOWN",  56 },
{ .name = "LEFTSHIFT",   57 },
{ .name = "X",           58 },
{ .name = "V",           59 },
{ .name = "N",           60 },
{ .name = ",",           61 },
{ .name = "/",           62 },
{ .name = "RUNSTOP",     63 },

{ .name = "ENTER",        8 },
{ .name = "INST",         0 },
{ .name = "INSTDEL",      0 },
{ .name = "LEFTARROW",   15 },
{ .name = "RIGHTCURSOR", 16 },
{ .name = "CLR",         30 },
{ .name = "CLRHOME",     30 },
{ .name = "SHIFTRIGHT",  38 },
{ .name = "CBM",         47 },
{ .name = "C=",          47 },
{ .name = "UPARROW",     54 },
{ .name = "DOWNCURSOR",  56 },
{ .name = "SHIFTLEFT",   57 },
{ .name = "RUN",         63 },
{ .name = "STOP",        63 },
{ .name = "SEMICOLON",   22 },
{ .name = "CTRL",        23 },
{ .name = "ASTERISK",    14 },
{ .name = "PLUS",         5 },
{ .name = "COMMA",       61 },
{ .name = "MINUS",       29 },
{ .name = "SLASH",       62 },
{ .name = "EQUAL",       46 },
{ .name = "DOT",         37 },
{ .name = "AT",          53 },
};

#endif // KEYS_H

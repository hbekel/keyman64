#ifndef KEYS_H
#define KEYS_H

#include "config.h"

typedef struct {
  char name[32];
  Key key;
} Symbol;

Symbol symbols[64+17] = {
  
{ .name = "DEL",         .key = { .row = 0, .col = 0 } },
{ .name = "THREE",       .key = { .row = 0, .col = 1 } },
{ .name = "FIVE",        .key = { .row = 0, .col = 2 } },
{ .name = "SIX",         .key = { .row = 0, .col = 3 } },
{ .name = "NINE",        .key = { .row = 0, .col = 4 } },
{ .name = "+",           .key = { .row = 0, .col = 5 } },
{ .name = "POUND",       .key = { .row = 0, .col = 6 } },
{ .name = "ONE",         .key = { .row = 0, .col = 7 } },

{ .name = "RETURN",      .key = { .row = 1, .col = 0 } },
{ .name = "W",           .key = { .row = 1, .col = 1 } },
{ .name = "R",           .key = { .row = 1, .col = 2 } },
{ .name = "Y",           .key = { .row = 1, .col = 3 } },
{ .name = "I",           .key = { .row = 1, .col = 4 } },
{ .name = "P",           .key = { .row = 1, .col = 5 } },
{ .name = "*",           .key = { .row = 1, .col = 6 } },
{ .name = "ARROWLEFT",   .key = { .row = 1, .col = 7 } },

{ .name = "CURSORRIGHT", .key = { .row = 2, .col = 0 } },
{ .name = "A",           .key = { .row = 2, .col = 1 } },
{ .name = "D",           .key = { .row = 2, .col = 2 } },
{ .name = "G",           .key = { .row = 2, .col = 3 } },
{ .name = "J",           .key = { .row = 2, .col = 4 } },
{ .name = "L",           .key = { .row = 2, .col = 5 } },
{ .name = ";",           .key = { .row = 2, .col = 6 } },
{ .name = "CONTROL",     .key = { .row = 2, .col = 7 } },

{ .name = "F7",          .key = { .row = 3, .col = 0 } },
{ .name = "FOUR",        .key = { .row = 3, .col = 1 } },
{ .name = "SIX",         .key = { .row = 3, .col = 2 } },
{ .name = "EIGHT",       .key = { .row = 3, .col = 3 } },
{ .name = "ZERO",        .key = { .row = 3, .col = 4 } },
{ .name = "-",           .key = { .row = 3, .col = 5 } },
{ .name = "HOME",        .key = { .row = 3, .col = 6 } },
{ .name = "TWO",         .key = { .row = 3, .col = 7 } },

{ .name = "F1",          .key = { .row = 4, .col = 0 } },
{ .name = "Z",           .key = { .row = 4, .col = 1 } },
{ .name = "C",           .key = { .row = 4, .col = 2 } },
{ .name = "B",           .key = { .row = 4, .col = 3 } },
{ .name = "M",           .key = { .row = 4, .col = 4 } },
{ .name = ".",           .key = { .row = 4, .col = 5 } },
{ .name = "RIGHTSHIFT",  .key = { .row = 4, .col = 6 } },
{ .name = "SPACE",       .key = { .row = 4, .col = 7 } },

{ .name = "F3",          .key = { .row = 5, .col = 0 } },
{ .name = "S",           .key = { .row = 5, .col = 1 } },
{ .name = "F",           .key = { .row = 5, .col = 2 } },
{ .name = "H",           .key = { .row = 5, .col = 3 } },
{ .name = "K",           .key = { .row = 5, .col = 4 } },
{ .name = "COLON",       .key = { .row = 5, .col = 5 } },
{ .name = "=",           .key = { .row = 5, .col = 6 } },
{ .name = "COMMODORE",   .key = { .row = 5, .col = 7 } },

{ .name = "F5",          .key = { .row = 6, .col = 0 } },
{ .name = "E",           .key = { .row = 6, .col = 1 } },
{ .name = "T",           .key = { .row = 6, .col = 2 } },
{ .name = "U",           .key = { .row = 6, .col = 3 } },
{ .name = "O",           .key = { .row = 6, .col = 4 } },
{ .name = "@",           .key = { .row = 6, .col = 5 } },
{ .name = "ARROWUP",     .key = { .row = 6, .col = 6 } },
{ .name = "Q",           .key = { .row = 6, .col = 7 } },

{ .name = "CURSORDOWN",  .key = { .row = 7, .col = 0 } },
{ .name = "LEFTSHIFT",   .key = { .row = 7, .col = 1 } },
{ .name = "X",           .key = { .row = 7, .col = 2 } },
{ .name = "V",           .key = { .row = 7, .col = 3 } },
{ .name = "N",           .key = { .row = 7, .col = 4 } },
{ .name = ",",           .key = { .row = 7, .col = 5 } },
{ .name = "/",           .key = { .row = 7, .col = 6 } },
{ .name = "RUNSTOP",     .key = { .row = 7, .col = 7 } },

{ .name = "ENTER",       .key = { .row = 1, .col = 0 } },
{ .name = "INST",        .key = { .row = 0, .col = 0 } },
{ .name = "INSTDEL",     .key = { .row = 0, .col = 0 } },
{ .name = "LEFTARROW",   .key = { .row = 1, .col = 7 } },
{ .name = "RIGHTCURSOR", .key = { .row = 2, .col = 0 } },
{ .name = "CLR",         .key = { .row = 3, .col = 6 } },
{ .name = "CLRHOME",     .key = { .row = 3, .col = 6 } },
{ .name = "SHIFTRIGHT",  .key = { .row = 4, .col = 6 } },
{ .name = "CBM",         .key = { .row = 5, .col = 7 } },
{ .name = "C=",          .key = { .row = 5, .col = 7 } },
{ .name = "UPARROW",     .key = { .row = 6, .col = 6 } },
{ .name = "DOWNCURSOR",  .key = { .row = 7, .col = 0 } },
{ .name = "SHIFTLEFT",   .key = { .row = 7, .col = 1 } },
{ .name = "RUN",         .key = { .row = 7, .col = 7 } },
{ .name = "STOP",        .key = { .row = 7, .col = 7 } },
{ .name = "SEMICOLON",   .key = { .row = 2, .col = 6 } },
{ .name = "CTRL",        .key = { .row = 2, .col = 7 } },
};

#endif // KEYS_H

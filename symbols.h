#ifndef KEYS_H
#define KEYS_H

#include "config.h"
#include "keyboard.h"

typedef struct {
  char name[32];
  uint8_t key;
} Symbol;

Symbol symbols[64+31] = {
  
{ .name = "INSTDEL",     KEY_INSTDEL   }, // DEL
{ .name = "THREE",       KEY_3         },
{ .name = "FIVE",        KEY_5         },
{ .name = "SEVEN",       KEY_7         },
{ .name = "NINE",        KEY_9         },
{ .name = "+",           KEY_PLUS      },
{ .name = "POUND",       KEY_POUND     },
{ .name = "ONE",         KEY_1         },

{ .name = "RETURN",      KEY_RETURN    },
{ .name = "W",           KEY_W         },
{ .name = "R",           KEY_R         },
{ .name = "Y",           KEY_Y         },
{ .name = "I",           KEY_I         },
{ .name = "P",           KEY_P         },
{ .name = "*",           KEY_ASTERISK  },
{ .name = "BACKARROW",   KEY_BACKARROW }, // ARROWLEFT

{ .name = "CURSORLEFT", KEY_CURSORLEFT }, // CURSORRIGHT
{ .name = "A",           KEY_A          },
{ .name = "D",           KEY_D          },
{ .name = "G",           KEY_G          },
{ .name = "J",           KEY_J          },
{ .name = "L",           KEY_L          },
{ .name = ";",           KEY_SEMICOLON  },
{ .name = "CONTROL",     KEY_CONTROL    },

{ .name = "F7",          KEY_F7         },
{ .name = "FOUR",        KEY_4          },
{ .name = "SIX",         KEY_6          },
{ .name = "EIGHT",       KEY_8          },
{ .name = "ZERO",        KEY_0          },
{ .name = "-",           KEY_MINUS      },
{ .name = "HOME",        KEY_CLRHOME    },
{ .name = "TWO",         KEY_2          },

{ .name = "F1",          KEY_F1         },
{ .name = "Z",           KEY_Z          },
{ .name = "C",           KEY_C          },
{ .name = "B",           KEY_B          },
{ .name = "M",           KEY_M          },
{ .name = ".",           KEY_PERIOD     },
{ .name = "SHIFTRIGHT",  KEY_SHIFTRIGHT }, // RIGHTSHIFT
{ .name = "SPACE",       KEY_SPACE      },

{ .name = "F3",          KEY_F3         },
{ .name = "S",           KEY_S          },
{ .name = "F",           KEY_F          },
{ .name = "H",           KEY_H          },
{ .name = "K",           KEY_K          },
{ .name = "COLON",       KEY_COLON      },
{ .name = "=",           KEY_EQUAL      },
{ .name = "COMMODORE",   KEY_CBM        },

{ .name = "F5",          KEY_F5         },
{ .name = "E",           KEY_E          },
{ .name = "T",           KEY_T          },
{ .name = "U",           KEY_U          },
{ .name = "O",           KEY_O          },
{ .name = "@",           KEY_AT         },
{ .name = "ARROWUP",     KEY_UPARROW    },
{ .name = "Q",           KEY_Q          },

{ .name = "CURSORDOWN",  KEY_CURSORDOWN },
{ .name = "SHIFTLEFT",   KEY_SHIFTLEFT  }, // LEFTSHIFT
{ .name = "X",           KEY_X          },
{ .name = "V",           KEY_V          },
{ .name = "N",           KEY_N          },
{ .name = ",",           KEY_COMMA      },
{ .name = "/",           KEY_SLASH      },
{ .name = "RUNSTOP",     KEY_RUNSTOP    },

{ .name = "ENTER",       KEY_RETURN     },
{ .name = "INST",        KEY_INSTDEL    },
{ .name = "DEL",         KEY_INSTDEL    },
{ .name = "LEFTARROW",   KEY_BACKARROW  },
{ .name = "ARROWLEFT",   KEY_BACKARROW  },
{ .name = "RIGHTCURSOR", KEY_CURSORLEFT },
{ .name = "CURSORRIGHT", KEY_CURSORLEFT },
{ .name = "LEFTCURSOR",  KEY_CURSORLEFT },
{ .name = "CLR",         KEY_CLRHOME    },
{ .name = "CLRHOME",     KEY_CLRHOME    },
{ .name = "SHIFTRIGHT",  KEY_SHIFTRIGHT },
{ .name = "RIGHTSHIFT",  KEY_SHIFTRIGHT },
{ .name = "CBM",         KEY_CBM        },
{ .name = "C=",          KEY_CBM        },
{ .name = "UPARROW",     KEY_UPARROW    },
{ .name = "DOWNCURSOR",  KEY_CURSORDOWN },
{ .name = "SHIFT",       KEY_SHIFTLEFT  },
{ .name = "SHIFTLEFT",   KEY_SHIFTLEFT  },
{ .name = "LEFTSHIFT",   KEY_SHIFTLEFT  },
{ .name = "RUN",         KEY_RUNSTOP    },
{ .name = "STOP",        KEY_RUNSTOP    },
{ .name = "SEMICOLON",   KEY_SEMICOLON  },
{ .name = "CTRL",        KEY_CONTROL    },
{ .name = "ASTERISK",    KEY_ASTERISK   },
{ .name = "PLUS",        KEY_PLUS       },
{ .name = "COMMA",       KEY_COMMA      },
{ .name = "MINUS",       KEY_MINUS      },
{ .name = "SLASH",       KEY_SLASH      },
{ .name = "EQUAL",       KEY_EQUAL      },
{ .name = "DOT",         KEY_PERIOD     },
{ .name = "AT",          KEY_AT         },
};

#endif // KEYS_H

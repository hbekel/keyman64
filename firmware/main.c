#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>

#define MC 1<<PA0
#define MD 1<<PA1

static volatile uint8_t matrix[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static void SetupHardware(void) {
  
  // Control / Output 8-11
  DDRA  = 0b11110001;
  PORTA = 0b00001110;

  // CIA1 PORT A
  DDRB  = 0b00000000;
  PORTB = 0b11111111;

  // Output 0-7
  DDRC  = 0b11111111;
  PORTC = 0b00000000;

  // CIA1 PORT B
  DDRD  = 0b11111111;
  PORTD = 0b00000000;

  InitUART();

  printf("Hardware initialized\n");
}

static void ClockMatrix(void) {
  PORTA |= MC;
  PORTA &= ~MC;
}

static void DebugMatrix(void) {
  uint8_t row = 0;
  uint8_t col = 0;

  for(row=0; row<8; row++) {
    for(col=0; col<8; col++) {
      printf("%s ", (matrix[row] & col) ? "1" : "0");
    }
    printf("\n");
  }
}

int main(void) {
  SetupHardware();

  uint8_t row = 0;
  uint8_t col = 0;
  
  while(1) {

    for(row=0; row<8; row++) {
      for(col=0; col<8; col++) {

	if((PINA & MD) == 0) {
	  matrix[row] &= ~(1<<col);
	}
	else {
	  matrix[row] |= 1<<col;
	}
	ClockMatrix();
      }
    }
    DebugMatrix();
  }  
  return 0;
}

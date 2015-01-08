#include "Arduino.h"
#include "AD9850.h"
#define pulse(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
#define REFERENCE_CLOCK 125e6 // 125 MHz

void AD9850::setup(int w_clk, int fq_ud, int data, int reset) {
  W_CLK = w_clk;
  FQ_UD = fq_ud;
  DATA = data;
  RESET = reset;

  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);

  pulse(RESET);
  pulse(W_CLK);
  pulse(FQ_UD);
}

void AD9850::oscillate(double frequency) {
  // defined in datasheet pg. 8 as
  // f_out = (deltaPhase * CLKIN)/2^32
  unsigned long deltaPhase = frequency * pow(2,32) / REFERENCE_CLOCK;

  // 40 bit total serial message
  // 32 bits of frequency LSB first

  for (int i=0; i<4; i++, deltaPhase>>=8) {
    shiftOut(DATA, W_CLK, LSBFIRST, deltaPhase & 0xFF);
  }

  // internal test, powerdown and phase bits
  shiftOut(DATA, W_CLK, LSBFIRST, B0000);
  shiftOut(DATA, W_CLK, LSBFIRST, B0000);

  pulse(FQ_UD);
}

void AD9850::stop() {

  // 2 bits of 0 for internal test codes
  // 1 bit for power down
  // 5 bits for phase LSB first
  // 0010 0000

  pulse(FQ_UD);
  shiftOut(DATA, W_CLK, LSBFIRST, B0100);
  shiftOut(DATA, W_CLK, LSBFIRST, B0000);
  pulse(FQ_UD);
}

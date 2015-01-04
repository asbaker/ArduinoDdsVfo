#include "AD9850.h"
 
#define W_CLK 2       // Pin 8 - connect to AD9850 module word load clock pin (CLK)
#define FQ_UD 3       // Pin 9 - connect to freq update pin (FQ)
#define DATA 4       // Pin 10 - connect to serial data load pin (DATA)
#define RESET 5      // Pin 11 - connect to reset pin (RST).

AD9850 vfo;
 
void setup() {
  vfo.setup(W_CLK, FQ_UD, DATA, RESET);
}
 
void loop() {
 // vfo.sendFrequency(10.e6);
  vfo.sendFrequency(1000);  // freq
  while(1);
}




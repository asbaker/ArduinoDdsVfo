#ifndef AD9850_h
#define AD9850_h

#include "Arduino.h"

class AD9850 {
  public:
    void setup(int w_clk, int fq_ud, int data, int reset);
    void oscillate(double frequency);
    void stop();
  private:
    int W_CLK;
    int FQ_UD;
    int DATA;
    int RESET;
    void sendToChip(double frequency, boolean powerDown);
};

#endif

#ifndef AD9850_h
#define AD9850_h

/*
 * A simple single freq AD9850 Arduino test script
 * Original AD9851 DDS sketch by Andrew Smallbone at www.rocketnumbernine.com
 * Modified for testing the inexpensive AD9850 ebay DDS modules
 * Pictures and pinouts at nr8o.dhlpilotcentral.com
 * 9850 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9850.pdf
 * Use freely
 */



#include "Arduino.h"

class AD9850 {
  public:
    void setup(int w_clk, int fq_ud, int data, int reset);
    void sendFrequency(double frequency);
  private:
    int W_CLK;
    int FQ_UD;
    int DATA;
    int RESET;
    void tfr_byte(byte data);




};

#endif
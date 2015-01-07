#include "AD9850.h"
#include "Keypad.h"

#define DEFAULT_HZ 1000
double currentFrequency = DEFAULT_HZ;
double newFrequency = 0;

/*
States:
 * Not Oscillating
 * Oscillating
 * Frequency Input
*/

// TODO: Implement statemachine to control oscillator from keypad

#define STATE_OSC "oscillating"
#define STATE_NOT_OSC "not oscillating"
#define STATE_FREQ_IN "frequency input"



void oscillateOnEnter() {
  Serial.println("entering oscillate on");

  updateLcd(currentFrequency);
}
void oscillateOnUpdate(char key) {
  Serial.print("update for oscillate on");
  respondToOscillateKeys(key);
}


void oscillateOffEnter() {
  Serial.println("entering oscillate off");

  updateLcd(currentFrequency);
}
void oscillateOffUpdate(char key) {
  Serial.print("update for oscillate off");
  Serial.println(key);
  respondToOscillateKeys(key);
}

void respondToOscillateKeys(char key) {
  Serial.print("respondToOscillateKeys: ");
  Serial.println(key);

  if(key == '#') {
    stateMachine.immediateTransitionTo(frequencyInput);
  }
  else if(key == '*') {
    if(stateMachine.isInState(oscillateOn)) {
      Serial.println("is in state oscillate on");
      stateMachine.immediateTransitionTo(oscillateOff);
    }
    else if(stateMachine.isInState(frequencyInput)) {
      Serial.println("is in state frequency input");
      stateMachine.immediateTransitionTo(oscillateOff);
    }
    else {
      Serial.println("is in state oscillate on");
      stateMachine.immediateTransitionTo(oscillateOn);
    }
  }
}


void frequencyInputEnter() {
  Serial.println("frequency input enter");
  newFrequency = 0;
  updateLcd(newFrequency);
}
void frequencyInputUpdate(char key) {
  Serial.print("frequency input update");
  Serial.println(key);
}


void printFrequency(double freq) {
  Serial.println(freq);
}
void updateLcd(double freq) {
  Serial.println("-------------------------");
  Serial.print("Hz: ");
  printFrequency(currentFrequency);

  if(stateMachine.isInState(oscillateOff)) {
    Serial.print("( )");
  }
  else if(stateMachine.isInState(oscillateOn)) {
    Serial.print("(~)");
  }

  Serial.println("  *-toggle #-freq");
  Serial.println("-------------------------");
}





#define W_CLK 2       // Pin 8 - connect to AD9850 module word load clock pin (CLK)
#define FQ_UD 3       // Pin 9 - connect to freq update pin (FQ)
#define DATA 4       // Pin 10 - connect to serial data load pin (DATA)
#define RESET 5      // Pin 11 - connect to reset pin (RST).

#define ROWS 4
#define COLS 3
byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {10, 11, 12};

char keys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

AD9850 vfo;

void setup() {
  Serial.begin(9600); 
  vfo.setup(W_CLK, FQ_UD, DATA, RESET);

  keypad.addEventListener(keypadEvent);
  Serial.println("done with setup");
}
 
void loop() {
 // vfo.sendFrequency(10.e6);
  //vfo.sendFrequency(1000);  // freq
  if(stateMachine.isInState(noop)) {
    Serial.println("tranisitioning to oscillate off");
    stateMachine.immediateTransitionTo(oscillateOff);
  }

  char key = keypad.getKey();
  // stateMachine.update();
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    lastKeyPress = key;

    if(keypad.getState() == PRESSED) {
      Serial.print("last key pressed: ");
      Serial.println(lastKeyPress);

      stateMachine.update();
    }
}
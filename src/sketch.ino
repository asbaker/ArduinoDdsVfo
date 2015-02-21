#include "AD9850.h"
#include "Keypad.h"
#include "State.h"
#include "serLCD.h"
#include "Button.h"
#include "ClickEncoder.h"
#include "TimerOne.h"

ClickEncoder *encoder;
int16_t last, value;

void timerIsr() {
  encoder->service();
}

#define DEFAULT_HZ 1000
long currentFrequency = DEFAULT_HZ;
long newFrequency = 0;
long multiplier = 1;

long startFrequency = 0;
long stopFrequency = 0;
long time = 0;
long lastMillis = 0;


AD9850 vfo;
serLCD lcd(13);

#define W_CLK 2
#define FQ_UD 3
#define DATA  4
#define RESET 5

State notOscillating(notOscillatingEnter, NULL);
State oscillating(oscillatingEnter, NULL);
State frequencyInput(frequencyInputEnter, respondToFrequencyKeys);

// states for auto sweep
// start freq -> end freq -> time -> auto sweep -> no oscillating
State startFrequencyInput(startFrequencyEnter, respondToStartKeys);
State endFrequencyInput(stopFrequencyEnter, respondToStopKeys);
State timeInput(timeEnter, respondToTimeKeys);
State autoSweep(autoSweepEnter, NULL);
State manualSweep(manualSweepEnter, NULL);


StateMachine stateMachine = StateMachine();


byte rowPins[4] = {12, 11, 10, 9};
byte colPins[3] = {8, 7, 6};

char keys[4][3] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 3);


Button oscillateButton = Button(A0, BUTTON_PULLUP_INTERNAL);
Button frequencyButton = Button(A1, BUTTON_PULLUP_INTERNAL);
Button autoSweepButton = Button(A2, BUTTON_PULLUP_INTERNAL);
Button manualSweepButton = Button(A3, BUTTON_PULLUP_INTERNAL);

void setup() {
  Serial.begin(9600);
  vfo.setup(W_CLK, FQ_UD, DATA, RESET);


  keypad.addEventListener(keypadEvent);

  oscillateButton.clickHandler(oscillatePressed);
  frequencyButton.clickHandler(frequencyPressed);
  autoSweepButton.clickHandler(autoSweepPressed);
  manualSweepButton.clickHandler(manualSweepPressed);

  stateMachine.changeState(notOscillating);

  encoder = new ClickEncoder(A5, A4, A7, 4);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
}

void loop() {
  char key = keypad.getKey();

  oscillateButton.isPressed();
  frequencyButton.isPressed();
  autoSweepButton.isPressed();
  manualSweepButton.isPressed();


  if(stateMachine.is(autoSweep)) {
    updateAutoSweep();
  }
  if(stateMachine.is(manualSweep)) {
    respondToSpin(encoder->getValue());
  }
}

void updateAutoSweep() {
  int hzPerSecond = (stopFrequency-startFrequency)/time;
  int millisPassed = millis() - lastMillis; // thousands of a second
  lastMillis = millis();

  int addToFrequency = (hzPerSecond/1000.*millisPassed);
  currentFrequency = currentFrequency + addToFrequency;

  if(currentFrequency>stopFrequency) {
    stateMachine.changeState(notOscillating);
  }
  else {
    vfo.oscillate(currentFrequency);
    updateLcd(currentFrequency);
    delay(10);
  }
}


/* ####### BUTTON INTERACTIONS ####### */

void oscillatePressed(Button& button) {
  if(stateMachine.is(notOscillating)) {
    stateMachine.changeState(oscillating);
  }
  else {
    stateMachine.changeState(notOscillating);
  }
}

void frequencyPressed(Button& button) {
  stateMachine.changeState(frequencyInput);
}

void autoSweepPressed(Button& button) {
  stateMachine.changeState(startFrequencyInput);
}

void manualSweepPressed(Button& button) {
  if(stateMachine.is(manualSweep)) {
    if(multiplier==10000000) {
      multiplier = 1;
    }
    else {
      multiplier*=10;
    }
    updateManualSweepLcd(currentFrequency);
  }
  else {
    stateMachine.changeState(manualSweep);
  }

}


/* ########## KEYPAD INTERACTIONS ######### */

void keypadEvent(KeypadEvent key){
  if(keypad.getState() == PRESSED) {
    stateMachine.sendKey(key);
  }
}

// not sure how to best refactor these ....
// curryable functions would be nice about right now

void respondToFrequencyKeys(char key) {
  switch (key) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      newFrequency = newFrequency*10 + getKeyInt(key);
      updateLcd(newFrequency);
      break;
    case '#':
      currentFrequency = newFrequency;
      stateMachine.changeState(notOscillating);
      break;
  }
}

void respondToStartKeys(char key) {
  switch (key) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      startFrequency = startFrequency*10 + getKeyInt(key);
      updateStartLcd();
      break;
    case '#':
      stateMachine.changeState(endFrequencyInput);
      break;
  }
}

void respondToStopKeys(char key) {
  switch (key) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      stopFrequency = stopFrequency*10 + getKeyInt(key);
      updateStopLcd();
      break;
    case '#':
      stateMachine.changeState(timeInput);
      break;
  }
}

void respondToTimeKeys(char key) {
  switch (key) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      time = time*10 + getKeyInt(key);
      updateTimeLcd();
      break;
    case '#':
      stateMachine.changeState(autoSweep);
      break;
  }
}

int getKeyInt(char key) {
  return (int)key - (int)'0';
}

void respondToSpin(int diff) {
  if(diff != 0) {
    currentFrequency = currentFrequency+(diff*multiplier);
    vfo.oscillate(currentFrequency);

    updateManualSweepLcd(currentFrequency);
  }

}

/* ########################################## */


/* ######## STATE MANAGEMENT #########*/

void notOscillatingEnter() {
  vfo.stop(); 
  updateLcd(currentFrequency);
}

void oscillatingEnter() {
  vfo.oscillate(currentFrequency);
  updateLcd(currentFrequency);
}

void frequencyInputEnter() {
  newFrequency = 0;
  updateLcd(newFrequency);
}

void startFrequencyEnter() {
  startFrequency = 0;
  updateStartLcd();
}

void stopFrequencyEnter() {
  stopFrequency = 0;
  updateStopLcd();
}

void timeEnter() {
  time = 0;
  updateTimeLcd();
}

void autoSweepEnter() {
  currentFrequency=startFrequency;
  lastMillis = millis();
}

void manualSweepEnter() {
  updateManualSweepLcd(currentFrequency);
  vfo.oscillate(currentFrequency);
}


/* ################################## */


/* ######## LCD UPDATING ########### */

void updateLcd(long freq) {
  lcd.clear();
  if(stateMachine.is(notOscillating)) {
    lcd.print("(    ) ");
  }
  else if(stateMachine.is(oscillating)) {
    lcd.print("(v^v^) ");
  }
  printFrequency(freq);

  if(stateMachine.is(frequencyInput)) {
    lcd.print("#-save");
  }
  else if(stateMachine.is(autoSweep)) {
    lcd.print("Auto Sweep...");
  }
}

void printFrequency(long freq) {
  lcd.print(freq);
  lcd.selectLine(2);
}

void updateStartLcd() {
  lcd.clear();
  printFrequency(startFrequency);
  lcd.print("Start Hz  #-save");
}

void updateStopLcd() {
  lcd.clear();
  printFrequency(stopFrequency);
  lcd.print("End Hz    #-save");
}

void updateTimeLcd() {
  lcd.clear();

  lcd.print(time);
  lcd.selectLine(2);

  lcd.print("Seconds   #-save");
}

void updateManualSweepLcd(long currentFrequency) {
  lcd.clear();
  lcd.print("(v^v^) ");
  printFrequency(currentFrequency);
  lcd.print("Sweep * ");
  lcd.print(multiplier);
}

/* #################*/

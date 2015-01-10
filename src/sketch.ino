#include "AD9850.h"
#include "Keypad.h"
#include "State.h"
#include "serLCD.h"
#include "Button.h"

#define DEFAULT_HZ 1000
long currentFrequency = DEFAULT_HZ;
long newFrequency = 0;


AD9850 vfo; 
#define W_CLK 2
#define FQ_UD 3
#define DATA  4
#define RESET 5

State notOscillating(notOscillatingEnter, NULL);
State oscillating(oscillatingEnter, NULL);
State frequencyInput(frequencyInputEnter, respondToFrequencyKeys);;
StateMachine stateMachine = StateMachine();


byte rowPins[4] = {6, 7, 8, 9};
byte colPins[3] = {10, 11, 12};

char keys[4][3] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 3);

serLCD lcd(13);


Button oscillateButton = Button(A0, BUTTON_PULLUP_INTERNAL);
Button frequencyButton = Button(A1, BUTTON_PULLUP_INTERNAL);

void setup() {
  Serial.begin(9600);
  Serial.println("starting setup");
  vfo.setup(W_CLK, FQ_UD, DATA, RESET); 


  keypad.addEventListener(keypadEvent);
  oscillateButton.clickHandler(oscillatePressed);
  frequencyButton.clickHandler(frequencyPressed);

  stateMachine.changeState(notOscillating);
  Serial.println("done with setup");
}

void loop() {
  char key = keypad.getKey();

  oscillateButton.isPressed();
  frequencyButton.isPressed();
}

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

/* ########## KEYPAD INTERACTIONS ######### */

void keypadEvent(KeypadEvent key){
  Serial.println("keypad event");
  if(keypad.getState() == PRESSED) {
    Serial.print("key pressed: ");
    Serial.println(key);

    stateMachine.sendKey(key);
  }
}

void respondToFrequencyKeys(char key) {
  Serial.print("respondToFrequencyKeys: ");
  Serial.println(key);

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
      newFrequency = newFrequency*10 + (int)key - (int)'0';
      updateLcd(newFrequency);
      break;
    case '#':
      currentFrequency = newFrequency;
      stateMachine.changeState(notOscillating);
      break;
  }
}

/* ########################################## */


/* ######## STATE MANAGEMENT #########*/

void notOscillatingEnter() {
  Serial.println("entering oscillate off");
  vfo.stop(); 
  updateLcd(currentFrequency);
}

void oscillatingEnter() {
  Serial.println("entering oscillate on");
  vfo.oscillate(currentFrequency);
  updateLcd(currentFrequency);
}

void frequencyInputEnter() {
  Serial.println("frequency input enter");
  newFrequency = 0;
  updateLcd(newFrequency);
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
  else {
    lcd.print("*-toggle #-freq");
  }
}

void printFrequency(long freq) {
  lcd.print(freq);
  lcd.selectLine(2);
}

/* #################*/

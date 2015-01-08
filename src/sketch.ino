#include "AD9850.h"
#include "Keypad.h"
#include "State.h"
#include "serLCD.h"

#define DEFAULT_HZ 1000
long currentFrequency = DEFAULT_HZ;
long newFrequency = 0;


AD9850 vfo; 
#define W_CLK 2
#define FQ_UD 3
#define DATA  4
#define RESET 5

State notOscillating(notOscillatingEnter, respondToOscillateKeys);
State oscillating(oscillatingEnter, respondToOscillateKeys);
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


void setup() {
  Serial.begin(9600);
  Serial.println("starting setup");
  vfo.setup(W_CLK, FQ_UD, DATA, RESET); 
  analogWrite(A0, 128);



  keypad.addEventListener(keypadEvent);

  stateMachine.changeState(notOscillating);
  Serial.println("done with setup");
}

void loop() {
  char key = keypad.getKey();
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

void respondToOscillateKeys(char key) {
  Serial.print("respondToOscillateKeys: ");
  Serial.println(key);

  if(key == '#') {
    stateMachine.changeState(frequencyInput);
  }
  else if(key == '*') {
    if(stateMachine.is(oscillating)) {
      Serial.println("is in state oscillate on");
      stateMachine.changeState(notOscillating);
    }
    else if(stateMachine.is(frequencyInput)) {
      Serial.println("is in state frequency input");
      stateMachine.changeState(notOscillating);
    }
    else {
      Serial.println("is in state oscillate on");
      stateMachine.changeState(oscillating);
    }
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
  /* 1234567890123456 */
  /* ( ) 14 070 000hz */
  /* #-save           */
  /* *-toggle #-freq  */


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

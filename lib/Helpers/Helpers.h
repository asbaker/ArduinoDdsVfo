Keypad setupKeypad(void (*eventFn)(KeypadEvent)) {
  byte rowPins[4] = {6, 7, 8, 9};
  byte colPins[3] = {10, 11, 12};

  char keys[4][3] = {
      {'1','2','3'},
      {'4','5','6'},
      {'7','8','9'},
      {'*','0','#'}
  };
  Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 3);
  keypad.addEventListener(eventFn);

  return keypad;
}


void printFrequency(double freq) {
  Serial.println(freq);
}


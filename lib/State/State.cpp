#include "Arduino.h"
#include "State.h"

State::State(void (*startFn)(), void (*keyFn)(char)) {
  // Serial.println("state init");
  _startFn = startFn;
  _keyFn = keyFn;
}

void State::start() {
  // Serial.println("invoking state start");
  _startFn();
}

void State::sendKey(char key) {
  // Serial.println("invoking send key");
  if(_keyFn != NULL) {
    _keyFn(key);
  }
}

bool StateMachine::is(State& state) {
  // Serial.println("ising");
  return &state == _currentState;
}

void StateMachine::sendKey(char key) {
  // Serial.println("sending key!");
  _currentState->sendKey(key);
}

void StateMachine::changeState(State &state) {
  // Serial.println("changing state");
  _currentState = &state;
  _currentState->start();
}

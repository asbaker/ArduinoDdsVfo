#include "Arduino.h"
#include "State.h"

State::State(void (*startFn)(), void (*keyFn)(char)) {
  _startFn = startFn;
  _keyFn = keyFn;
}

void State::start() {
  _startFn();
}

void State::sendKey(char key) {
  _keyFn(key);
}





StateMachine::StateMachine(State& state) {
  changeState(state);
}

bool StateMachine::is(State& state) {
  return &state == _currentState;
}

void StateMachine::sendKey(char key) {
  _currentState->sendKey(key);
}

void StateMachine::changeState(State &state) {
  _currentState = &state;
  _currentState->start();
}

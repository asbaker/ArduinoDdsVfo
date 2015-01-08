#ifndef STATE_H
#define STATE_H

#include "Arduino.h"

class State {
  public:
    State( void (*startFn)(), void (*keyFn)(char));
    void start();
    void sendKey(char key);

  private:
    void (*_startFn)();
    void (*_keyFn)(char);

};



class StateMachine {
  public:
    StateMachine(State& state);
    bool is(State& state);
    void sendKey(char key);
    void changeState(State &state);

  private:
    State* _currentState;
};



#endif

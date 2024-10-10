/* motorLogic.h - Library for logic control of motors */

#ifndef motorLogic_h
#define motorLogic_h

#include <Arduino.h>

class MotorLogic
{
  public:
    MotorLogic(int in1_R, int in2_R, int in1_L, int in2_L);  // Constructor
    void forward();   // Move forward
    void backward();  // Move backward
    void cwTurn();    // Zero-radius clockwise turn
    void ccwTurn();   // Zero-radius counter-clockwise turn
    void stop();      // Stop all motors
  private:
    int _in1R, _in2R, _in1L, _in2L;  // Motor control pins
};

#endif  // motorLogic_h

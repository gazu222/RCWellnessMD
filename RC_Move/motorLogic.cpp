// motorLogic.cpp
#include "motorLogic.h"

// Constructor: initialize motor control pins
MotorLogic::MotorLogic(int in1R, int in2R, int in1L, int in2L) {
  _in1R = in1R;
  _in2R = in2R;
  _in1L = in1L;
  _in2L = in2L;

  // Set the pin modes for motor control pins
  pinMode(_in1R, OUTPUT);
  pinMode(_in2R, OUTPUT);
  pinMode(_in1L, OUTPUT);
  pinMode(_in2L, OUTPUT);
}

// Move both motors forward
void MotorLogic::forward() {
  digitalWrite(_in1R, HIGH); digitalWrite(_in2R, LOW);  // Right side forward
  digitalWrite(_in1L, HIGH); digitalWrite(_in2L, LOW);  // Left side forward
}

// Move both motors backward
void MotorLogic::backward() {
  digitalWrite(_in1R, LOW); digitalWrite(_in2R, HIGH);  // Right side backward
  digitalWrite(_in1L, LOW); digitalWrite(_in2L, HIGH);  // Left side backward
}

// Zero-radius turn right (left forward, right backward)
void MotorLogic::cwTurn() {
  digitalWrite(_in1R, LOW); digitalWrite(_in2R, HIGH);  // Right side backward
  digitalWrite(_in1L, HIGH); digitalWrite(_in2L, LOW);  // Left side forward
}

// Zero-radius turn left (right forward, left backward)
void MotorLogic::ccwTurn() {
  digitalWrite(_in1R, HIGH); digitalWrite(_in2R, LOW);  // Right side forward
  digitalWrite(_in1L, LOW); digitalWrite(_in2L, HIGH);  // Left side backward
}

// Stop both motors
void MotorLogic::stop() {
  digitalWrite(_in1R, LOW); digitalWrite(_in2R, LOW);  // Right side stop
  digitalWrite(_in1L, LOW); digitalWrite(_in2L, LOW);  // Left side stop
}

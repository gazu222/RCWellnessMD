// 2 motor drivers and 4 motors
//     Motor Driver 1
// Motor A: Front right side. corresponds to Ain1_R and Ain2_R
// Motor B: Rear right side. corresponds to BIn1_R and BIn_R
//    Motor Driver 2
// Motor A: front left side. corresponds to Ain1_L and Ain2_L
// Motor B: Rear left side. corresponds to Bin1_L and Bin2_L
//
// Motor Driver input combinations:
// applies to Ain pins or Bin pins
// Motor Short Brake:       Ain1 = HIGH, Ain2 = HIGH
// Motor Stop:              Ain1 = LOW,  Ain2 = LOW
// Motor Clockwise:         Ain1 = HIGH, Ain2 = LOW
// Motor Counter Clockwise: Ain1 = LOW,  Ain2 = HIGH

//******************Important*************************
// Ain1 is tied to Bin1
// Ain2 is tied to Bin2
// I.e. the way the car is put together electornically
// The motors on the right side will both turn forward based
// on 2 pins from arduino
//*****************************************************

#include <math.h>
#include "motorLogic.h"  // Include the motorLogic library

// Motor control pin definitions
int in1_R = 7;
int in2_R = 8;
int in1_L = 5;
int in2_L = 6;

// Create a MotorLogic object for the car's motor control
MotorLogic car(in1_R, in2_R, in1_L, in2_L);

// PWM pins
int en_R = 9;
int en_L = 3;

// Max and min duty cycles for PWM
const int maxPWM = 255;
const int minPWM = 128;

void setup() {
  Serial.begin(9600);

  // Setup PWM pins if you are using them
  pinMode(en_R, OUTPUT);
  pinMode(en_L, OUTPUT);

  //Initialize pwm signals to zero
  analogWrite(en_R,0);
  analogWrite(en_L,0);

  // Initialize all motors to stop
  car.stop();
}

void loop() {
  // Example Test Case 5: Zero-radius right turn
 // Serial.println("Test Case 5: Zero-radius right turn");
 digitalWrite(in1_R,HIGH);
 digitalWrite(in2_R,LOW);
 digitalWrite(en_R, HIGH);

 digitalWrite(in1_L,HIGH);
 digitalWrite(in2_L,LOW);
 digitalWrite(en_L, HIGH);

 
;

  // Additional test cases can be added here similarly...
}

void Move(int y_axis, int x_axis) {
  const int TOL = 20;  // Tolerance for zero-radius turn
  const int TOL2 = 5;  // Tolerance for stopping the car

  // Stop condition (when both y_axis and x_axis are below TOL2)
  if (abs(y_axis) < TOL2 && abs(x_axis) < TOL2) {
    car.stop();  // Stop all motors using the library
    Serial.println("Motors stopped");
    return;
  }

  // Handle zero-radius turn if y_axis is much smaller than x_axis
  if (abs(y_axis) < TOL && abs(x_axis) >= TOL) {
    if (x_axis > 0) {
      car.cwTurn();  // Zero-radius turn to the right
      Serial.println("Zero-radius turn right");
    } else {
      car.ccwTurn();  // Zero-radius turn to the left
      Serial.println("Zero-radius turn left");
    }
    return;
  }

  // Handle forward and backward motion using veering logic
  if (y_axis > 0) {
    car.forward();  // Move forward
    Serial.println("Moving forward");
  } else {
    car.backward();  // Move backward
    Serial.println("Moving backward");
  }
}

// Logarithmic scaling function (if you still want to use PWM)
int logScale(int input, int minPWM, int maxPWM) {
  int magnitude = abs(input);
  if (magnitude == 0) return 0;

  float scaled = log10(magnitude);
  float maxLog = log10(maxPWM);

  int scaledOutput = (scaled / maxLog) * (maxPWM - minPWM) + minPWM;
  return constrain(scaledOutput, minPWM, maxPWM);
}

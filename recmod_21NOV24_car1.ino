/*
Basic code for reciving data on the car, from the controller
tested 10/11 preliminarily working
*/
#include <math.h>
#include "motorLogic.h"  // Include the motorLogic library
#include <RFM69.h>
#include <SPI.h>
#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      2   // My node ID (0 to 255)
#define TONODEID      255   // Destination node ID (0 to 254, 255 = broadcast)
#define LED           9

// Motor control pin definitions
int in1_R = 8;
int in2_R = 7;
int in1_L = 5;
int in2_L = 6;

RFM69 radio;

// Create a MotorLogic object for the car's motor control
MotorLogic car(in1_R, in2_R, in1_L, in2_L);

// PWM pins
int en_R = 9;
int en_L = 3;

// Max and min duty cycles for PWM
const int maxPWM = 255; //~6V
const int minPWM = 128; //~4V
void setup()
 {
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");
  //pinMode(LED,OUTPUT);
  //digitalWrite(LED,LOW);
  radio.initialize(RF69_915MHZ,MYNODEID,NETWORKID); //radio startup
  radio.setHighPower();
  //declare pinmodes
  pinMode(in1_R, OUTPUT);
  pinMode(in2_R, OUTPUT);
  pinMode(in1_L, OUTPUT);
  pinMode(in2_L, OUTPUT);
  pinMode(en_R, OUTPUT);
  pinMode(en_L, OUTPUT);

  //Initialize pwm signals to zero
  analogWrite(en_R,0);
  analogWrite(en_L,0);

  // Initialize all motors to stop
  car.stop();
}

void loop()
{
int x=0;
int y=0;
static char message[]="";

  if(radio.receiveDone())
  {
    for (int i=0;i<9;i++) // get chars from radio
    { 
      message[i]=((char)radio.DATA[i]);
    }
  
    x=getx(message);
    y=gety(message);

    Move(x,y);
    /*
    Serial.print("(");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.println(")");
    */
  
  }
  delay(25);
 

}
int getx(char input[])
{
  int x=0;
  x=atoi(input);
  return(x);
}

int gety(char input[])
{
  static int reclength = 4; //num of character in each value
  int y=0;
  static char recmess[]="";

  for (int i=0;i<reclength+1;i++) // get chars from radio into recmess array
  { 
    recmess[i]=((char)input[i+4]);
  }
  y=atoi(recmess);
  return(y);
}

void blink(int pin) //blink function to pulse a pin
{
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
  return;
}

void Move(int y_axis, int x_axis) {
    const int MOVE_TOL = 50;           // Movement tolerance
    const float VEER_SCALE = 0.15;      // Veer smoothing tolerance

    // Initialize PWM outputs
    int pwm_R = 0;
    int pwm_L = 0;

    // Apply logarithmic scaling to inputs
    int pwm_y = logScale(y_axis, minPWM, maxPWM);  // Base PWM for forward/backward
    int pwm_x = logScale(x_axis, minPWM, maxPWM);  // Base PWM for turning

    // Stop condition
    if (abs(y_axis) <= MOVE_TOL && abs(x_axis) <= MOVE_TOL) {
        car.stop();
        analogWrite(en_R, 0);
        analogWrite(en_L, 0);
        Serial.println("Motors stopped");
        return;
    }

    // Straight movement: y-axis dominant, x-axis below tolerance
    if (abs(x_axis) < MOVE_TOL) {
        pwm_R = pwm_y;
        pwm_L = pwm_y;
        if (y_axis > 0) {
            car.forward();
            Serial.println("Moving forward");
        } else {
            car.backward();
            Serial.println("Moving backward");
        }
    }
    // Zero-radius turn: x-axis dominant, y-axis below tolerance
    else if (abs(y_axis) < MOVE_TOL) {
        pwm_R = pwm_x;
        pwm_L = pwm_x;
        if (x_axis > 0) {
            car.cwTurn();
            Serial.println("Zero-radius turn right");
        } else {
            car.ccwTurn();
            Serial.println("Zero-radius turn left");
        }
    }
    // Veering: Both x and y above tolerance
    else {
        pwm_R = pwm_y;
        pwm_L = pwm_y;

        // Apply quadratic veering adjustment
        int veer_adjustment = VEER_SCALE * (pow(abs(x_axis), 2) / pow(256, 2)) * pwm_y;
        
        if (x_axis > 0) {
            pwm_R = max(pwm_R - veer_adjustment, minPWM);  // Veer right, reduce right motor speed
            Serial.println("Veer right");
        } else {
            pwm_L = max(pwm_L - veer_adjustment, minPWM);  // Veer left, reduce left motor speed
            Serial.println("Veer left");
        }

        // Set movement direction based on y-axis
        if (y_axis > 0) {
            car.forward();
            Serial.println("Moving forward with veer");
        } else {
            car.backward();
            Serial.println("Moving backward with veer");
        }
    }

    // Update motor PWM values
    analogWrite(en_R, int(round(pwm_R)));
    analogWrite(en_L, int(round(pwm_L)));

    // Debugging motor PWM values
    Serial.print("PWM Right: ");
    Serial.println(pwm_R);
    Serial.print("PWM Left: ");
    Serial.println(pwm_L);
}



// Logarithmic scaling function for PWM)
int logScale(int input, int minPWM, int maxPWM) {
  int magnitude = abs(input);
  if (magnitude == 0) return 0;

  float scaled = log10(magnitude);
  float maxLog = log10(maxPWM);

  int scaledOutput = (scaled / maxLog) * (maxPWM - minPWM) + minPWM;
  return constrain(scaledOutput, minPWM, maxPWM);
}
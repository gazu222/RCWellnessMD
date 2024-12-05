#include <MPU6050.h>
#include <RFM69.h>
#include <SPI.h>
#include <Wire.h>

RFM69 radio;
MPU6050 accelgyro;

// (x,y,z) vectors
  int16_t xD, yD, zD;

// number of frames AKA sliding windows to compute
  const byte frameAmt = 63;

// array to store x & y values
  int dataStore[frameAmt][2];

// create the scalars for (x,y) averages and (x,y) offsets
  float xOFF;
  float yOFF;
  float xAVG;
  float yAVG;

// array to store the offset values for a threshold of ±16g
  int offsetStore[frameAmt][2];

// max & min values to establish range of motion
  float max = 2048;
  float min = -1*max;

// establish the maximum range of values: (±x,±y), and
// create a second bound that is divided for ease of computation
  float bound = 2*frameAmt-1;
  float bound2 = round(bound/2);

// value to convert raw data into (x,y)
  int convertedValue;

// value for the roundMax() function to trigger
int trigMax = 230;

#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      255  // Destination node ID (0 to 254, 255 = broadcast)
#define FREQUENCY     RF69_915MHZ
#define OUTPUT_READABLE_ACCELGYRO

void setup() {
    // initialize serial communication
    Serial.begin(9600);

    // initialize device
    accelgyro.initialize();

    // verify connection
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // Set accelerometer to ±16g
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); 

  // for initial setup, calculate the average value over 63 frames
  // the (x,y) values calculated will be used to normalize final axis
  for (int i = 0; i<frameAmt;i++) 
      {
        // read raw accel/gyro measurements from device
        accelgyro.getAcceleration(&xD, &yD, &zD);

        // store those offset values into an array
        offsetStore[i][1] = xD;
        offsetStore[i][2] = yD;

        // sum the values
        xOFF = xOFF + offsetStore[i][1];
        yOFF = yOFF + offsetStore[i][2];
      }

      // divide by N number of terms
        xOFF = xOFF/frameAmt;
        yOFF = yOFF/frameAmt;
      
      // scale values to +/-255 and round
        xOFF = convert(xOFF);
        yOFF = convert(yOFF);
  }

void loop() 
{   

    for (int i = 0; i<frameAmt;i++) 
      {
        // read raw accel/gyro measurements from device
          accelgyro.getAcceleration(&xD, &yD, &zD);
          dataStore[i][1] = xD;
          dataStore[i][2] = yD;

        // sum a frame of x & y values for a moving average
          xAVG = xAVG + dataStore[i][1];
          yAVG = yAVG + dataStore[i][2];
      }
    
    // complete time series averaging
      xAVG = xAVG/frameAmt;
      yAVG = yAVG/frameAmt;

    // finish taking average via division then scale relative to bounds
      xAVG = convert(xAVG)-xOFF;
      yAVG = convert(yAVG)-yOFF;

    // use contratin to prevent from going out of bounds
      xAVG = constrain(xAVG, -frameAmt*4-1, frameAmt*4-1);
      yAVG = constrain(yAVG, -frameAmt*4-1, frameAmt*4-1);

    // this function will check if the value is greater/less than +/-230
    // if it is, it rounds it to +/-255
      xAVG = roundMax(xAVG);
      yAVG = roundMax(yAVG);
      
    // print the values
      Serial.print(" (");
      Serial.print(xAVG);
      Serial.print(",");
      Serial.print(yAVG);
      Serial.println(" )");

    // send the value 
    Send(xAVG,yAVG);
    
      delay(50);
  
}

// function to convert raw data into (x,y) format
int convert(int a) 
{

  // convert input to a more human readable value and then round
  convertedValue = 4*((((a - min)/(max - min))*bound)-bound2);
  convertedValue = round(convertedValue);

  return convertedValue;

}

// function to round and return the max value when a threshold is met
int roundMax(int b) 
{

  if (b>=trigMax)
  {
    return b=255;
  }
  else if(b<=-trigMax)
  {
    return b=-255;
  }
  else {
    return b;
  }
  
}


// Kyle's send function
  void Send(int x, int y)
  {
    String xpart ="";
    String ypart ="";
    int messSize = 9;
    char message[messSize];
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
  //format x componet
    if(x>-10 && x<0)
    {
      xpart= "-00"+String(abs(x));
    }
    else if (x<-9 && x>-100)
    {
      xpart= "-0"+String(abs(x));
    }
    else if (x<-99)
    {
      xpart="-"+String(abs(x));
    }
    else if (x>-1 && x<10)
    {
      xpart="+00"+String(x);
    }
    else if (x>9 && x<100)
    {
      xpart="+0"+String(x);
    }
    else if (x>99)
    {
      xpart="+"+String(x);
    }

  // format y componet
    if(y>-10 && y<0)
    {
      ypart= "-00"+String(abs(y));
    }
    else if (y<-9 && y>-100)
    {
      ypart= "-0"+String(abs(y));
    }
    else if (y<-99)
    {
      ypart="-"+String(abs(y));
    }
    else if (y>-1 && y<10)
    {
      ypart="+00"+String(y);
    }
    else if (y>9 && y<100)
    {
      ypart="+0"+String(y);
    }
    else if (y>99)
    {
      ypart="+"+ String(y);
    }
    String hold= xpart + ypart;
    hold.toCharArray(message, messSize);
    Serial.println(message);
    radio.send(TONODEID, message, messSize);
    return;
  }
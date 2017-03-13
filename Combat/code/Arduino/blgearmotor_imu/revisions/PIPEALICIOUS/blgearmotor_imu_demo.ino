#include <Servo.h>

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define C_DEG 180. / PI   // C_DEG

// MPU DMP control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
//float euler[3];         // [psi, theta, phi]    Euler angle container
VectorInt16 gyro;

Servo m0;
Servo m1;

bool debug = true;
unsigned long mill = 0;

void setup() {
 
  if(debug) Serial.begin(115200);

  // Configure MPU
  mpu_setup();

  m0.attach(6 );
  m1.attach(5 );
  
  if(debug) Serial.println("writing...");
  m0.write(0);  m1.write(0);
  delay(1000); //delay 1 second,  some speed controllers may need longer
  m0.write(90);  m1.write(90);
  delay(1000); //delay 1 second,  some speed controllers may need longer
  m0.write(0);  m1.write(0);
  delay(1000); //delay 1 second,  some speed controllers may need longer
  if(debug) Serial.println("done");

  pinMode(13, OUTPUT);
  
  if(debug) Serial.println("Done with setup");
  unsigned long mill = millis();
}

static int minPWM = 1027;     // by inspection
static int maxPWM = 1985;     // by inspection
float midPWM      = 1488.;    // by inspection
float steeringGain = 1.;
float expo = 3.;           // expo coefficient
float limit = 0;             // travel limit in percentage
float yawP = 0.2;
bool deathSpin = false;           // TODO if steering command is saturated low or high, spin at full speed in that drection. This is your attack. 
float inverted = 0.;      // sign of gravity vector. ASSUMES MPU sitting on ground plane. 

void loop() {
  deathSpin = false;
  if (!dmpReady) return;
  mpu_block();
  if (!mpu_overflow())
  {
    int  steerRaw    = pulseIn( 7, HIGH, 25000); // 20000 occasionally has zeros -> moved this to expose PWM pin
    int  throttleRaw = pulseIn( 8, HIGH, 25000);
    bool imuMode     = pulseIn( 9, HIGH, 25000) > 1500;

    if ((steerRaw > maxPWM) || (steerRaw < minPWM))
    {
      deathSpin = true;
    }
     
    // Map to -1..1 and constrain to limits
    float steerStick    = mapf(steerRaw,    minPWM, maxPWM, -1.0+limit, 1.0-limit);
    float throttleStick = mapf(throttleRaw, minPWM, maxPWM, -1.0+limit, 1.0-limit); // OK
//    Serial.print(steerStick);Serial.print(" ");Serial.println(throttleStick); 

    // Apply expo
    steerStick    = signf(steerStick)    * pow(abs(steerStick),    expo);
    throttleStick = signf(throttleStick) * pow(abs(throttleStick), expo);

    // set command to neutral
    int leftCmd  = midPWM;
    int rightCmd = midPWM; 
    
    if ((steerRaw==0) || (throttleRaw==0)) // inhibit if we lost signal
    {
      m0.writeMicroseconds(leftCmd);
      m1.writeMicroseconds(rightCmd);
  //    if (debug) Serial.println("Remove before flight inhibit");
    } else { 
      if (!imuMode) 
      {
        digitalWrite(13, LOW);
        leftCmd  += (throttleStick + steerStick) * (494.-(float)limit); 
        rightCmd += (throttleStick - steerStick) * (494.-(float)limit);
        
      } else {
        digitalWrite(13, HIGH);

        // detect our orientation
        inverted = -1. * signf(gravity.z);

        leftCmd  += throttleStick * (494.-(float)limit) * (inverted * -1.); 
        rightCmd += throttleStick * (494.-(float)limit) * (inverted * -1.);
        
        // use guided solution
        // gyro_z is positive right hand rule (z is up 
        Serial.print("YPR: ");
        Serial.print(ypr[0]*C_DEG); Serial.print(" ");Serial.println(gyro.z);
        leftCmd  += (gyro.z * yawP * inverted) + (steerStick*steeringGain) * (494.-(float)limit);
        rightCmd -= (gyro.z * yawP * inverted) + (steerStick*steeringGain) * (494.-(float)limit);

        Serial.print("Gravity: ");
        Serial.print(inverted); Serial.print(" ");Serial.println(gravity.z);
        
      }

      if(deathSpin)
      {
        Serial.println("DEATHSPIN ACTIVATED");
        if (leftCmd < rightCmd)
        {
          leftCmd  = minPWM;
          rightCmd = maxPWM;  
        } else {
          leftCmd  = maxPWM;
          rightCmd = minPWM;  
        }
      }
      
      m0.writeMicroseconds(leftCmd);
      m1.writeMicroseconds(rightCmd);   
    }
  
    if (debug) 
    {
       Serial.print("Mode ");Serial.println(imuMode);
       Serial.print("L: ");Serial.print(leftCmd); Serial.print(" ");Serial.print(steerStick); Serial.print(" ");Serial.print(steerRaw); Serial.println(" steer ");  // throttle
       Serial.print("R: ");Serial.print(rightCmd);Serial.print(" ");Serial.print(throttleStick);Serial.print(" ");Serial.print(throttleRaw);Serial.println(" throttle ");  // elevator
       Serial.print("Loop time: ");Serial.println(millis() - mill);
    }
    mill = millis();
  }
}

static inline float signf(float val) {
 if (val < 0) return -1.;
 if (val==0) return 0.;
 return 1.;
}

float mapf(long x, long in_min, long in_max, float out_min, float out_max)
{
  // TODO: note this does not constrain
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}


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

// MPU raw outputs
//int16_t ax, ay, az;
//int16_t gx, gy, gz;

// complementary filter
//float pitch       =0;

// controls
float angle0;           // target angle
float angle;              // angle from vertical
float angleLast;          // previous angle from vertical

// PID
float Kp =100.;            // proportional
float Kd =80.;//10.;            // derivative
float Ki =4;//2.;            // integral 10 integral was bang-bang and spazzing
/* WAS: 80/80/4 */

// 60/0/4 -> 1 hour balance
// Find minimal Kp, then minimal Ki, then add a litle Kd...
// 30/0/2.5 wanders a bit w/75 min pid mapping. With 25 deadzone 75 top a bit herky jerky but longest on solid floor to date. try 25/50
// not sure I can balance on hardwood with my current sloppy motors

// HARDWOOD: 80/0/4 stopping point w/no dead zone, no mapping, smallwheels

int killAngle = 30;   // +/- 70 kill motor to not grind self into ground
bool pause = false;   // serial pause

int cmd = 0;

typedef struct MOTOR 
{
  int pwm;
  int fwd;
  int rev;
  float gain;
  int cmd;
};


MOTOR left  = {10, 8, 9,1.,0};
MOTOR right = {11,13,12,.8,0}; 

//unsigned long timer;

// ================================================================
// ===               FLOAT SIGN FUNCTION                        ===
// ================================================================
float sign(float x)
{
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  // Open serial
  Serial.begin(115200);
  while (!Serial); // wait for Leonardo enumeration, others continue immediately

  // Init motors
  pinMode(left.pwm, OUTPUT);
  pinMode(left.fwd, OUTPUT);
  pinMode(left.rev, OUTPUT);

  pinMode(right.pwm, OUTPUT);
  pinMode(right.fwd, OUTPUT);
  pinMode(right.rev, OUTPUT);

  // Configure MPU
  mpu_setup();

  // Configure balbot code
  angle0 = 0.5; // 1.4; //1.45;  // positive arduino side
//  timer = millis();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  mpu_block();
  if (!mpu_overflow())
  {
//   long int dt = micros() - timer;
//    ComplementaryFilter(ax, ay, az, gy, gz, dt);
// angle = angle0 - pitch;
//    timer += dt;

    // balance bot code //
    angle = (ypr[2] * C_DEG);
    
    cmd = pidController(angle0, angle, angleLast, Kp, Kd, Ki);
    angleLast = angle;

    if (abs(angle) > killAngle || pause) {
      cmd = 0;
    }

//    if (abs(cmd) > 25) {
//      cmd = sign(cmd)*map(abs(cmd),0,255,50,255); // I think this helps, 50-60ish
//   }

    left.cmd  = cmd * left.gain; 
    right.cmd = cmd * right.gain;
    
    motor(left, right);
  }

  display_serial();
  check_serial();
}

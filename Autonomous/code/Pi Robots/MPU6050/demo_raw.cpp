#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "wiringPi.h"
#include <cmath>
#include <iostream>

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

unsigned int  _time;
double  timePrev, timeStep;
double gyroScale = 131;
int i = 1;
double arx, ary, arz, grx, gry, grz, gsx, gsy, gsz, rx, ry, rz;

void setup() {
    // initialize device
    printf("Initializing I2C devices...\n");
    accelgyro.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(accelgyro.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    printf("Setting up wiringPi\n");
    wiringPiSetupSys();
    _time = millis();
}

void loop() {
    // set up time for integration
    timePrev = (double)_time;
    _time = millis();
    timeStep = ((double)_time - timePrev) / 1000.; // time-step in s

    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

// apply gyro scale from datasheet
  gsx = gx/gyroScale;   gsy = gy/gyroScale;   gsz = gz/gyroScale;

  // calculate accelerometer angles
  arx = (180/3.141592) * atan(ax / sqrt(ay*ay + az*az));
  ary = (180/3.141592) * atan(ay / sqrt(ax*ax + az*az));
  arz = (180/3.141592) * atan(sqrt(ay*ay + ax*ax) / az);
//  arz = (180/2.141592) * atan(az / sqrt(ax*ax + ay*ay));
  // set initial values equal to accel values
  if (i == 1) {
    grx = arx;
    gry = ary;
    grz = arz;
  }
  // integrate to find the gyro angle
  else{
    grx += (timeStep * gsx);
    gry += (timeStep * gsy);
    grz += (timeStep * gsz);
  }

  // apply filter
  rx = (0.96 * arx) + (0.04 * grx);
  ry = (0.96 * ary) + (0.04 * gry);
  rz = (0.96 * arz) + (0.04 * grz);
  i++;

  std::cout << rx << " " << ry << " " << rz << std::endl;
  delay(50);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // display accel/gyro x/y/z values
//   printf("a/g: %6hd %6hd %6hd   %6hd %6hd %6hd  %6hd\n",ax,ay,az,gx,gy,gz);
 //   msec = millis();
  //  delay( 50);
}

int main()
{
    setup();
    for (;;)
        loop();
}


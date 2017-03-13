/*
 * First hack stab and using the MPU9150, GPS and SD card reader simultaneously
 * 1. button for file saving/closing 
 * 2. file readback over serial (with button to trigger so we dont have to reprogram every time) (or timeout menu)
 * 2. #defines for debug stuff
 * 3. Fork TinyGPS++ and minimize
 * 4, See if sdfat is any smaller than adafruit
 * 5. Figure out why things slow down roughly every second - GPS NMEA reads account for most but not all of it.
 * 6. read file back over serial to host pc, see: https://www.arduino.cc/en/Tutorial/DumpFile
 * 7. Onboard filtering (iterate)
 * 8. order mega board
 */

 /*
  * MPU9150 demo: 20%
  * GPS 30%  - gainz here if you remove unused stuff
  * SD read/write: 40% - seems the gainz be here
  */

/// GENERAL /// 

/// GPS ///
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600; // static const gets optimized away
SoftwareSerial ss(RXPin, TXPin);    // The serial connection to the GPS device
TinyGPSPlus gps;                    // The TinyGPS++ object

/// MPU ///
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "I2Cdev.h"
#include "MPU9150.h"
//#include "helper_3dmath.h"

MPU9150 accelgyro;

/// SD ///
#include <SPI.h>
#include <SD.h>

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
static const int chipSelect = 10;
File dataFile;

/// ME ///
int i = 0;                             // iteration count
static const int fileSwitchPin = 9.;   // switch for toggling file saves
unsigned long previousMillis = 0;      // timing

// TODO eval types. can save stuff as ints instead of doubles, just need to convert
struct packet {
  int16_t ax, ay, az;   // accel
  int16_t gx, gy, gz;   // gyro
  int16_t mx, my, mz;   // mag
  double lat, lon, alt; // gps lat/lon/alt
  double hdng, vel;     // gps heading and velocity
  uint32_t time;        // gps time
  int dt;               // time from last iter
} p;


void setup()
{
  /// GPS ///
  Serial.begin(115200);
  ss.begin(GPSBaud);
  /// END GPS ///

  /// MPU ///
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin(); // ok

  // initialize device
  accelgyro.initialize();
  /// END MPU ///

  /// SD ///
  Serial.println(F("Initializing SD card..."));
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SS, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    //while (1) ;
        blink_forever();
  }
  
  // Open up the file we're going to log to!
  dataFile = sd_unique_fh();
  /// END SD ///

  // File save toggle // 
  pinMode(fileSwitchPin, INPUT);

  /// timing! /// 
  previousMillis = millis();

  // TODO if you can get a GPS time quickly (seconds) then wait on gps time
  // use gps date/time to stamp file name instead of iterating

  /// TODO file readback option if serial connected - use if(serial) {}
/*      // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again
*/
}


void loop()
{
  i++;

  while(millis() - 20 < previousMillis) {}
  p.dt = (millis() - previousMillis);
  previousMillis += p.dt;
  
  /// GPS /// 
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read())) {}

  if (gps.location.isUpdated()) {
    p.lat  = gps.location.lat();
    p.lon  = gps.location.lng();
  }
  if (gps.altitude.isUpdated()) {
    p.alt  = gps.altitude.meters();  
  }
  if (gps.course.isUpdated()) {
    p.hdng = gps.course.deg();
  }
  if (gps.speed.isUpdated()) {
    p.vel  = gps.speed.mps();
  }
  if (gps.time.isUpdated()) {
    p.time = gps.time.value();
  }
  /// END GPS ///

  /// MPU ///
  // TODO occasional hiccup here bumps from 100-130msec regardless of base rate. Even w/latest libraries
  // TODO would using interrupt fix?
  accelgyro.getMotion9(&p.ax, &p.ay, &p.az, &p.gx, &p.gy, &p.gz, &p.mx, &p.my, &p.mz);
  // accelgyro.getMotion6(&p.ax, &p.ay, &p.az, &p.gx, &p.gy, &p.gz); // this hiccups too 
  /// END MPU /// 

  /// SD /// 
  char buf[100];
  sprintf(buf,"%i %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %d, %d, %d, %d,\n",  p.dt, p.time, p.ax, p.ay, p.ax, p.gx, p.gy, p.gz, p.mx, p.my, p.mz, p.lat, p.lon, p.alt, p.hdng, p.time);
  // TODO doubles arent printing right
  dataFile.print(buf);
  Serial.print(buf);
/*
  /// SAVE FILE BUTTON /// 
  if ((digitalRead(fileSwitchPin) == LOW) && (i>10)) { // TODO 2 of first 3 iters switch is zero.
    Serial.println(F("Saving File"));
    dataFile.close();
    while(digitalRead(fileSwitchPin) == LOW){}
    Serial.println(F("Opening new file"));
    dataFile = sd_unique_fh();
  }
  */    
  /// END SD ///
}

File sd_unique_fh()
{
  char filename[13];
  for (int j = 0; j < 1000; j++) {
    sprintf(filename,"NAV%05d.txt", j);
    if (!SD.exists(filename)) {
      File df = SD.open(filename, FILE_WRITE);
      if (! df) {
        Serial.println(F("Card failed, or not present"));
        //while(1) {}
        blink_forever();
      }
      return df;
    }
  }
}


void read_file_serial(char * fname)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(fname);

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening "); Serial.println(fname);
  }
}

void blink_forever()
{
  pinMode(13, OUTPUT);
  while(1)
 {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
}
}


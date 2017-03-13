/*
  Arduino Neato XV-11 Laser Distance Scanner
  Motor control board v0.2 by Cheng-Lung Lee
  XV-11 LDS adapter reads LDS data from RX3 then relay to TX. Also extract the speed
  data from the data stream to do speed control on LDS motor. Everythin can power
  from USB no extra power required.
  Change log:
  V0.2 Add simple speed control code update PWM 3 times per rev.
  V0.1 Opend loop control version.
  This code is tested on Arduino Mega 1280
  I/O:
  Motor drive by low side driver IPS041L connect to PWM Pin4, Motor power from 5V
  Neato XV-11 LDS Vcc(red) : 5V
  Neato XV-11 LDS TX(Orange) : RX3

  // Inspiration / Information:
  // Reference 1: https://github.com/getSurreal/XV_Lidar_Controller
  // Reference 2: https://github.com/bombilee/NXV11/
  // Reference 3: http://profmason.com/?p=13246
  // Reference 4: https://xv11hacking.wikispaces.com/LIDAR+Sensor
*/
#include "LIDAR.h"

int DesiredRPM = 300; // Setting Desired RPM Here. WAS: 300 Try lower. 205 working best so far. 190 too low
const int MotorPWMPin = 9;
int inByte = 0;         // incoming serial byte
unsigned char Data_status = 0;
unsigned char Data_4deg_index = 0;
unsigned char Data_loop_index = 0;
unsigned char SpeedRPHhighbyte = 0; //
unsigned char SpeedRPHLowbyte = 0;
/*
    TODO
    - build packet instead of case switching
    - visualization on computer end
    - data processing on computer end
    - stable platform (fischertechnik)
    - wifi comms
    - rover implementation

*/
int SpeedRPH = 0;
const unsigned char PWM4dutyMax = 255;
const unsigned char PWM4dutyMin = 50;
unsigned char PWM4duty = 120; // have to set a default value make motor start spining

unsigned char inRange = 0;
int Range = 0;


  LIDAR LiDAR;
  unsigned char packet[22];
  PACKET pkt;
  
void setup() {
  pinMode(MotorPWMPin, OUTPUT);
  Serial.begin(115200);  // USB serial
  Serial3.begin(115200);  // XV-11 LDS data

  // prints title with ending line break
  Serial.println(F("Arduino Neato XV-11 Interface"));
  analogWrite(MotorPWMPin, 130 );

}

bool echo = true;

int counter = 0;
bool find_header = true;
void loop() {
 if (echo) {
      while (Serial3.available()) {
        Serial.write(Serial3.read());
    }
    while (Serial.available()) {
        Serial1.write(Serial.read());
    }
 } else {
    if (Serial3.available() > 0) {      // can do serial.available >=21 and read entire packet if you wanted. assuming you had alignment.
      inByte = Serial3.read();
      if(find_header){
        if (LiDAR.isHeader(inByte)) {   // start packet
          packet[counter++] = inByte;
          find_header = false;
        }
      } else {                          // continue packet
        packet[counter++] = inByte;
        if(counter == 21){
          counter = 0;
          // TODO try breaking up processing in-line, IE each step as the data are available... see if that helps
          // IE: state machine find_header, find_angle, find_speed, find_data_0, find_data_1 ... find_checksum
          find_header = true;
          // we have a complete packet
          pkt = LiDAR.readPacket(packet);
          // TODO serial prints are slow. try to minimize. Maybe get stats for 90 packets and blurt out.
          // but that means we have limited processing room too ... 
          Serial.print("Packet: ");Serial.print(pkt.number);Serial.print(" ");Serial.println(pkt.is_valid); // numbers good, stuff not valid. occasional missed pkt
        }
      }
    }
 }
}

/*
bool loopback = false;
bool echo     = false;

void loop() {
  if (loopback) {
    //// LOOPBACK TESTING FOR CODE ON PC ////
    while (Serial3.available()) {
        Serial.write(Serial3.read());
    }
    while (Serial.available()) {
        Serial1.write(Serial.read());
    }
    analogWrite(MotorPWMPin, 120 ); // update value
    
  } else {
    if (Serial3.available() > 0) {
      // get incoming byte:
      inByte = Serial3.read();

      if (echo) {
        //// ECHO BYTESTREAM ////
        Serial.println(inByte);
        if (inByte == 0xFA) {
          Serial.println("");
          Serial.print(count);
          Serial.print(" ");
          count = 0;
        }
        Serial.println(inByte, HEX);
        count++;
        Serial.print(" ");
        analogWrite(MotorPWMPin, 120 ); // update value
        } else {
          //// PACKET DECODER///
//          decodeData(inByte);
          if(LiDAR.isHeader(inByte)){
              packet[0] = inByte;
    //          for (int j = 1; j <= 21; j++) {
//                packet[j] = Serial3.read();
  //            }
              packet[1] = Serial3.read();
              Serial.print("PacketNumber: ");Serial.println(LiDAR.packetNumber(packet[1]));
              packet[2] = Serial3.read();
              packet[3] = Serial3.read();
              Serial.print("Speed: ");Serial.println(LiDAR.speed(packet[2],packet[3]));
              for(int i = 0; i < 4; i++) {
                unsigned char point_data[4];
                point_data[0] = Serial3.read();
                point_data[1] = Serial3.read();
                point_data[2] = Serial3.read();       /// looks like garbage compared to C impl! wtf? something w/serial.read?
                point_data[3] = Serial3.read();
                POINT p = LiDAR.point(point_data);
                Serial.print("point ");Serial.println(p.r);
          }
  //            pkt = LiDAR.readPacket(packet);
//              Serial.print("Packet: ");Serial.print(pkt.number);Serial.print(" ");Serial.println(pkt.is_valid);
      //        SpeedControl(pkt.speed);
              analogWrite(MotorPWMPin, 120 ); // update value
          }
        }
      }
    }
  }
*/
// Very simple speed control
void SpeedControl ( int RPMinput)
{
  if (Data_4deg_index == 0) { // I only do 3 updat I feel it is good enough for now
    if (SpeedRPH < RPMinput * 60)
      if (PWM4duty < PWM4dutyMax) PWM4duty++; // limit the max PWM make sure it don't overflow and make LDS stop working
    if (SpeedRPH > RPMinput * 60)
      if (PWM4duty > PWM4dutyMin) PWM4duty--; //Have to limit the lowest pwm keep motor running

    analogWrite(MotorPWMPin, PWM4duty ); // update value
  }
}

void check_serial()
{
  if (Serial.available())
  {
    char n = Serial.read();
    switch (n)
    {
      case 'i':       // mod setpoint
        PWM4duty--;
        break;
      case 'k':
        PWM4duty++;
        break;
    }
    Serial.println("DESIRED_RPM");
    Serial.println(PWM4duty);
    Serial.println("DESIRED_RPM");
  }
}

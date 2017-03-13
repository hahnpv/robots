#include <Servo.h>
 
Servo esc;

typedef struct MOTOR 
{
  int pwm;
  int fwd;
  int rev;
  float gain;
  int cmd;
};
/* D987 
 * D543
 * D6 -> special
 */
MOTOR left  = {11,9,10,1.,0};  // {9, 7, 8, 1., 0};
MOTOR right = {5,6,7,1.,0};     // {3, 5, 6, 1., 0}; 
int standby = 8;                // standy pin must be HIGH


bool debug = true;

void setup() {
 
  Serial.begin(115200);

  // Init motors
  pinMode(left.pwm, OUTPUT);
  pinMode(left.fwd, OUTPUT);
  pinMode(left.rev, OUTPUT);

  pinMode(right.pwm, OUTPUT);
  pinMode(right.fwd, OUTPUT);
  pinMode(right.rev, OUTPUT);

  // remove-before-flight
  // pinMode(2, INPUT_PULLUP);

  // standby
  pinMode(standby, OUTPUT);
  digitalWrite(standby, HIGH);    // for now. If you lose connection, take low.

  esc.attach(3 );
  
  Serial.println("writing...");
  esc.write(0);
  delay(1000); //delay 1 second,  some speed controllers may need longer
  esc.write(90);
  delay(1000); //delay 1 second,  some speed controllers may need longer
  esc.write(0);
  delay(1000); //delay 1 second,  some speed controllers may need longer
  Serial.println("done");
  
  Serial.println("Done with setup");
}

static int minPWM = 990;
static int maxPWM = 1980;

static int startupSpeed = 140;
static int runSpeed     = 60; // 45 doesnt maintain 90 too fast

bool lastSpinner = false;
unsigned long startupTime = 0;


void loop() {


//  bool remove_before_flight =  digitalRead(2) ? 0:1;

  int leftRaw  = pulseIn( 2, HIGH, 25000); // 20000 occasionally has zeros -> moved this to expose PWM pin
  int rightRaw = pulseIn( 4, HIGH, 25000);
  bool spinner = pulseIn(12, HIGH, 25000) > 1200;

  int leftStick  = map(leftRaw,  minPWM, maxPWM, -128, 128);   // throttle, percent
  int rightStick = map(rightRaw, minPWM, maxPWM, -255, 255);   // aileron, percent

  left.cmd  = constrain( leftStick + rightStick, -255, 255); 
  right.cmd = constrain( leftStick - rightStick, -255, 255); 

  if ((leftRaw==0) || (rightRaw==0)) // remove_before_flight)
  {
    left.cmd = 0;
    right.cmd = 0;
    motor(left, right);
    if (debug)
    {
      Serial.println("Remove before flight inhibit");
      delay(100);
    }
  } else
  { 
   motor(left, right);
  }

  drive_spinner(lastSpinner, spinner);
  lastSpinner = spinner;  

  if (debug) 
  {
     // NOTE can't delay with software servo or we f'up 
     delay(100);
     Serial.print("Spinner:");Serial.println(spinner);
     Serial.print(left.cmd); Serial.print(" ");Serial.print(leftStick); Serial.print(" ");Serial.print(leftRaw); Serial.println(" left ");  // throttle
     Serial.print(right.cmd);Serial.print(" ");Serial.print(rightStick);Serial.print(" ");Serial.print(rightRaw);Serial.println(" right ");  // elevator
  }
}


void drive_spinner(bool lastSpinner, bool spinner)
{
   if (!lastSpinner && spinner)
   {
      Serial.println("spinner startup");
      esc.write(startupSpeed);
      startupTime = millis();
   }  
   else if(spinner)
   {
      if ((millis() - startupTime) > 1000)
        esc.write(runSpeed);
      else
        esc.write(startupSpeed);
   }
   else if(lastSpinner && !spinner)
   {
     Serial.println("spinner shutdown");
     esc.write(0);
   }
}

// basic motor 
void motordir(MOTOR motor, bool cmd)
{
  if (cmd == true)
  {
    digitalWrite(motor.fwd, HIGH);
    digitalWrite(motor.rev, LOW);
  } else {
    digitalWrite(motor.fwd, LOW);
    digitalWrite(motor.rev, HIGH);
  }
}
void motor(MOTOR left, MOTOR right)
{

  motordir(left,  left.cmd  >0);
  motordir(right, right.cmd >0);

  analogWrite(left.pwm, abs(left.cmd));
  analogWrite(right.pwm, abs(right.cmd));
}



/*
 * 
 * NODEMCU VERSION OF CODE
 *
#include <ESP8266WiFi.h>

const char* ssid = "SBG65806A";
const char* password = "";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // motor
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);

  // remove before flight - this was weird. is 1,3 used with the motor driver chip?
//  pinMode(D1, INPUT_PULLUP);

  analogWrite(5, 0);
  analogWrite(4, 0);
  
  digitalWrite(0, 1);
  digitalWrite(2, 1);

  Serial.println("Done with setup");
}

static int minPWM = 1100;
static int maxPWM = 2000;

void loop() {
// while bind_plug_is_removed

if skid_steer
// This code works with reversed throttle
   int leftRaw = 0;
   int rightRaw = 0;

   int leftStick = pulseIn(D7, HIGH, 20000);
   int rightStick = pulseIn(D8, HIGH, 20000);
   
   int leftCmd  = map( leftStick, minPWM, maxPWM, -750, 750);
   int rightCmd = map( rightStick, minPWM, maxPWM, -750, 750);

    analogWrite(5, abs(leftCmd));             // 0-1023
    analogWrite(4, abs(rightCmd));            // 0-1023
    digitalWrite(0, leftCmd  >=0 ? 1 : 0);    // HIGH / LOW direction
    digitalWrite(2, rightCmd >=0 ? 1 : 0);

elseif throttle_direction
   int leftRaw  = pulseIn(D8, HIGH, 20000);
   int rightRaw = pulseIn(D6, HIGH, 20000)+104;
  
   int leftStick  = map(leftRaw, minPWM, maxPWM, -750, 750);   // throttle
   int rightStick = map(rightRaw, 950, 1600, -250, 250);   // aileron

   // these sign conversions make no damn sense... leftStick should be fixed magnitude.
   int leftCmd  = -leftStick - rightStick; // map( leftStick+rightStick, minPWM, maxPWM, -750, 750);
   int rightCmd = leftStick - rightStick; // map( leftStick-rightStick, minPWM, maxPWM, -256, 256);

    analogWrite(5, abs(leftCmd));             // 0-1023
    analogWrite(4, abs(rightCmd));            // 0-1023
    digitalWrite(0, leftCmd  >=0 ? 1 : 0);    // HIGH / LOW direction
    digitalWrite(2, rightCmd >=0 ? 1 : 0);
end
//
//   delay(100)
//   Serial.print(leftRaw);Serial.print(" ");Serial.print(leftStick);Serial.print(" ");Serial.print(leftCmd);Serial.print(" left ");Serial.println(leftCmd  >=0 ? 1 : 0);  // throttle
//   Serial.print(rightRaw);Serial.print(" ");Serial.print(rightStick);Serial.print(" ");Serial.print(rightCmd);Serial.print(" right ");Serial.println(rightCmd  >=0 ? 1 : 0);  // elevator
 //   }
}

 *
 *
 */

#include <Servo.h>

////// NOTE: UNTESTED REVISION OF WORKING SOFTWARE USING OTHER MOTOR DRIVER BOARD ///////
////// SHOULD WORK BUT MAY REQUIRE MINOR TWEAKS /////////////////////////////////////////

Servo esc;
 
// 5,6 and 3,11 for robot boards
//Motor left(5,6);    
//Motor right(3,11);    

typedef struct Motor
{
  int pin1;
  int pin2;
};
/* D987 
 * D543
 * D6 -> special
 */
Motor left  = {9,10};  // {9, 7, 8, 1., 0};
Motor right = {5, 6};     // {3, 5, 6, 1., 0}; 


bool debug = true;
bool servo = false;
void setup() {
  if(debug)
  {
      Serial.begin(115200);
  }

  pinMode(11, INPUT);
  pinMode(12, INPUT);

  if(servo)
  {

    esc.attach(2 );
    if(debug)
    {
      Serial.println("writing...");
    }
    esc.write(0);
    delay(1000); //delay 1 second,  some speed controllers may need longer
    esc.write(90);
    delay(1000); //delay 1 second,  some speed controllers may need longer
    esc.write(0);
    delay(1000); //delay 1 second,  some speed controllers may need longer
    if(debug)
    {
      Serial.println("done");
    }
  }

  if(debug)
  {
    Serial.println("Done with setup");
  }

}

static int minPWM = 990;
static int maxPWM = 1980;

static int startupSpeed = 90;
static int runSpeed     = 90; // 45 doesnt maintain 90 too fast

bool lastSpinner = false;
unsigned long startupTime = 0;

void loop() {
  int leftRaw  = pulseIn(12, HIGH, 25000); // 20000 occasionally has zeros -> moved this to expose PWM pin
  int rightRaw = pulseIn(11, HIGH, 25000);
  int spinInt = false;// pulseIn(  4, HIGH, 25000);

  bool spinner = spinInt > 1500;

  int leftStick  = map(leftRaw,  minPWM, maxPWM, -254, 254);   // throttle, percent
  int rightStick = map(rightRaw, minPWM, maxPWM, -254, 254);   // aileron, percent

  int leftCmd  = constrain( leftStick + rightStick, -254, 254); 
  int rightCmd = constrain( leftStick - rightStick, -254, 254); 


  if ((leftRaw==0) || (rightRaw==0)) // remove_before_flight)
  {
    motorSpeed(left,  0);
    motorSpeed(right, 0);

    if (debug)
    {
      Serial.println("Remove before flight inhibit");
      delay(100);
    }
  } else
  { 
    motorSpeed(left, leftCmd);
    motorSpeed(right, rightCmd);
  }

  if (servo)
  {
    drive_spinner(lastSpinner, spinner);
    lastSpinner = spinner;  
  }
  if (debug) 
  {
     delay(100);
     Serial.print("Spinner:");Serial.println(spinInt);
     Serial.print(leftCmd); Serial.print(" ");Serial.print(leftStick); Serial.print(" ");Serial.print(leftRaw); Serial.println(" left ");  // throttle
     Serial.print(rightCmd);Serial.print(" ");Serial.print(rightStick);Serial.print(" ");Serial.print(rightRaw);Serial.println(" right ");  // elevator
  }
}

void drive_spinner(bool lastSpinner, bool spinner)
{
   if (!lastSpinner && spinner)
   {
      if(debug)
      {
        Serial.println("spinner startup");
      }
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
     if(debug)
     {
       Serial.println("spinner shutdown");
     }
     esc.write(0);
   }
}

void motorSpeed(Motor m, int cmd) {
  if (cmd < 0) {
    analogWrite(m.pin1, abs(cmd));
    digitalWrite(m.pin2, LOW);
  }
  else if (cmd > 0) {
    digitalWrite(m.pin1, LOW);
    analogWrite(m.pin2, cmd);
  }
  else {
    digitalWrite(m.pin1, LOW);
    digitalWrite(m.pin2, LOW);
  }
}

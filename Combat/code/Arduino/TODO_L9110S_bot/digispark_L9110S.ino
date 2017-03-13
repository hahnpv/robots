typedef struct Motor
{
  int pwm;
  int dir;
};

typedef struct Receiver
{
  int drive;
  int steer;
};

/*
 * PCB FACING AWAY: 1,0 and 4,5
 * PCB FACING SAME DIRECTION: 0,1 and 5,4
 * 
 */

Motor left  = {0,1};   // 0,1 
Motor right = {5,4};  // 4,5

Receiver flysky = {2,3};  // 2,3,null,1

static int minPWM = 990;
static int maxPWM = 1980;
bool debug = true;

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(115200);
  pinMode( left.dir, OUTPUT );
  pinMode( left.pwm, OUTPUT );
  digitalWrite( left.dir, LOW );
  digitalWrite( left.pwm, LOW );

  pinMode( right.dir, OUTPUT );
  pinMode( right.pwm, OUTPUT );
  digitalWrite( right.dir, LOW );
  digitalWrite( right.pwm, LOW );

  pinMode( flysky.drive, INPUT);
  pinMode( flysky.steer, INPUT);
}
int spd = -250;
void loop() {
  int leftRaw  = pulseIn( flysky.drive, HIGH, 21000); // 20000 occasionally has zeros -> moved this to expose PWM pin
  int rightRaw = pulseIn( flysky.steer, HIGH, 21000);

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
  //    Serial.println("Remove before flight inhibit");
      delay(100);
    }
  } else
  { 


  motorSpeed(left,  leftCmd);
  motorSpeed(right, rightCmd);
  //Serial.println(spd);
  spd++;
  if(spd > 250)
  {
    spd = -250;  
  }
  delay(100);

    // Serial.print(leftCmd); Serial.print(" ");Serial.print(leftStick); Serial.print(" ");Serial.print(leftRaw); Serial.println(" left ");  // throttle
    // Serial.print(rightCmd);Serial.print(" ");Serial.print(rightStick);Serial.print(" ");Serial.print(rightRaw);Serial.println(" right ");  // elevator
  }
}



void motorSpeed(Motor m, int cmd) {
  int dir = cmd < 0 ? 0 : 1;
  cmd = abs(cmd);          // not needed
  //Serial.print( dir);Serial.print("  ");Serial.println(cmd);
  digitalWrite(m.dir, dir);
  if(dir)
  {
    analogWrite(m.pwm, 255-cmd);
  } else {
    analogWrite(m.pwm,     cmd);
  }
}


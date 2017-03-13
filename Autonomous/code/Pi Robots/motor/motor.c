#include <wiringPi.h>
#include <softPwm.h>

int main (void)
{
  wiringPiSetup () ;
  pinMode (24, OUTPUT);
//  pinMode (25, OUTPUT);
  softPwmCreate( 25, 0, 100);
  digitalWrite(24, LOW);
  for (int i = 0; i < 100; i++)
  {
//    digitalWrite (24, LOW) ;
//    digitalWrite (25, LOW)  ; delay (500) ;
//    digitalWrite (24,  LOW) ; 
//    digitalWrite (25, LOW) ; delay (500) ;
      softPwmWrite(25, i);
      delay( 50);
  }
  softPwmWrite(25, 0);
  delay( 50);
  return 0 ;
}

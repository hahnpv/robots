// basic motor 
void motor(MOTOR left, MOTOR right)
{
  if (left.cmd > 0 || right.cmd > 0)
  {
    digitalWrite(left.fwd, HIGH);
    digitalWrite(right.fwd, HIGH);
       
    digitalWrite(left.rev, LOW);
    digitalWrite(right.rev, LOW);
  } else {
    digitalWrite(left.fwd, LOW);
    digitalWrite(right.fwd, LOW);
       
    digitalWrite(left.rev, HIGH);
    digitalWrite(right.rev, HIGH);
  }
  analogWrite(left.pwm, abs(left.cmd));
  analogWrite(right.pwm, abs(right.cmd));
}


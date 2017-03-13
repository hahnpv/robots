// Serial read/write functions
int i = 0;       // constrain output

void display_serial()
{
  /*
  i++;
  if (i == 10)
  {
    Serial.print(millis() - oldMillis);
    Serial.print(" ");
    oldMillis = millis();

//    Serial.print(" CFP: ");Serial.print(pitch);
    Serial.print(" CMD: ");Serial.print(cmd);
    Serial.print(" Eul: ");Serial.print(euler[0]*C_DEG);Serial.print(", ");Serial.print(euler[1]*C_DEG);Serial.print(", ");Serial.print(euler[2]*C_DEG);Serial.print(", ");
    Serial.print(" PYR: ");Serial.print(ypr[0]*C_DEG);Serial.print(", ");Serial.print(ypr[1]*C_DEG);Serial.print(", ");Serial.print(ypr[2]*C_DEG);Serial.print(", ");
    Serial.print(" Angle: ");Serial.print(angle);
    Serial.print(" AngleLast: ");Serial.print(angle);
//    Serial.print(" CF: ");Serial.print(pitch);
    Serial.println("");

    i = 0;
  }
*/
}

void check_serial()
{
  if (Serial.available())
  {
    char n = Serial.read();
             switch (n)
    {
      case 'z':
        angle0 -= 0.1;
        break;
      case 'x':
        angle0 += 0.1;
        break;
      case 'e':
        Kp -= 1;
        break;
      case 'r':
        Kp += 1;
        break;
      case 'd':
        Kd -= 1;
        break;
      case 'f':
        Kd += 1;
        break;
      case 'c':
        Ki -= 0.1;
        break;
      case 'v':
        Ki += 0.1;
        break;
      case 'q':
        pause = !pause;
        Serial.print("Setpoint "); Serial.print(angle0);
        Serial.print("\tKp "); Serial.print(Kp);
        Serial.print("\tKd "); Serial.print(Kd);
        Serial.print("\tKi "); Serial.print(Ki);
        break;
    }
  }
}

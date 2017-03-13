float integral = 0; // integral term in PID
// ================================================================
// ===               PID Controller                             ===
// ================================================================
int j = 0;
int pidController(float setPoint, float currentPoint, float lastPoint, float Kp, float Kd, float Ki) {
  j++;
  float p = (setPoint - currentPoint) * Kp;
  float d = (-currentPoint + lastPoint) * Kd;         // (set - curr) - (set - last) 
  integral += ((setPoint - currentPoint) * Ki);
  integral = constrain(integral, -255., 255.);      // prevent windup

  if (j == 10)
  {
    Serial.print("P: "); Serial.print(p);Serial.print(" I: ");Serial.print(integral);Serial.print(" D: ");Serial.print(d);Serial.print(" angle: "); Serial.println(currentPoint);  
    j = 0;
    Serial.print("shouldnt integral and/or derivative incoroporate dt?");
  }
  
  int PID = (p + integral + d);
  return constrain(PID, -255., 255.);
}

// ================================================================
// ===               COMPLEMENTARY FILTER PROTOTYPE             ===
// ================================================================

/*
// https://forum.arduino.cc/index.php?topic=215997.0
void ComplementaryFilter(int ax,int ay,int az,int gy,int gz, long int delta_t) {
 long squaresum  = (long)ay*ay+(long)ax*ax;
 float pitchAcc  = atan(az/sqrt(squaresum))*RAD_TO_DEG;       // dt is micros not millis
 pitch +=((-gx/32.8f)*(delta_t/1000.0f));                  // TODO ensure gx is correct
 pitch  =0.98*pitch + (1.0f-0.98)*pitchAcc;
}
*/

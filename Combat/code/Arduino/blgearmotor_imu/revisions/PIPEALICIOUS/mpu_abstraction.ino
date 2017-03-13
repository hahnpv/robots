// Abstract away the MPU stuff so we can focus on code that matters

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
void dmpDataReady() {
  mpuInterrupt = true;
}


// ================================================================
// ===                          SETUP MPU                       ===
// ================================================================
void mpu_setup()
{
      // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  
  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

 
      // supply offsets. Script below works well:
      // http://www.i2cdevlib.com/forums/topic/96-arduino-sketch-to-automatically-calculate-mpu6050-offsets/
      mpu.setXGyroOffset(39);
      mpu.setYGyroOffset(3);
      mpu.setZGyroOffset(5);
      mpu.setXAccelOffset(-3905);
      mpu.setYAccelOffset(-1291);
      mpu.setZAccelOffset(1560);

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}

void mpu_block()
{
    // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {}

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

}

void mpu_data()
{
      // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    //// NOTE: ADdded this mid-May after ~ 1 month of lego/fischer bots. The MPU was 'flipping out' after 
    //// hard sign changes and spewing garbage; this seems to fix it. It does slow down the rate at which you get 
    //// data but since the MPU is integrating at a much higher rate this isn't an issue. IF YOU USE YOUR OWN COMPLIMENTARY
    //// FILTER THIS WILL BE A SHOW STOPPER! Not sure if my MPU is beat up, or if its a bad solder joint, or what. 
    //mpu.resetFIFO();
    //// NOTE: try resoldering connector, try new mpu6050
    //// A few weeks ago values started drifting, from startup to convergence took a few seconds I swear initially it 
    //// snapped to a correct value. may just be 'old'
    //// RESOLVED: replacing Arudino fixed issue.

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    // get data
    mpu.dmpGetQuaternion(&q, fifoBuffer);
//    mpu.dmpGetEuler(euler, &q);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    mpu.dmpGetGyro(&gyro,fifoBuffer);
//    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
}


bool mpu_overflow()
{
  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));
    return true;

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) { 
    mpu_data();    
  } else {
    Serial.println(F("Undefined status in mpu_overflow()!"));
  }
  return false;
}


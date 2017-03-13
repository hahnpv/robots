#ifndef SPECTRUMRECEIVER_H_
#define SPECTRUMRECEIVER_H_

// adaption of https://github.com/jbudworth/SpektrumSatRx to work with Redcon satellite
// information from https://www.spektrumrc.com/ProdInfo/Files/Remote%20Receiver%20Interfacing%20Rev%20A.pdf
// information from https://github.com/cleanflight/cleanflight/blob/master/src/main/rx/spektrum.c
// THIS CODE APPEARS TO WORK so long as you start up transmitter AFTER arduino is in loop() 
// the reason why is that it is difficult to discren byte order if you catch it mid-stream, there is no unique header.

#define MASK_1024_CHANID 0xFC00 


class SatelliteReceiver{
public:
SatelliteReceiver() {
  missCnt = 0;
}
void getFrame(void){
  if(Serial.available() > 16)
  {
    while(Serial.available() > 16)
    {
     missCnt = 0;
     uint8_t         spek_chan_shift = 2;
     uint8_t         spek_chan_mask = 0x03;
      
      for(index = 0; index <= 15; index++)
      {
        inByte = Serial.read();
        inData[index] = inByte;
      
  //      Serial.print(inByte);
  //    Serial.print(" ");
      }
  //  Serial.println(" ");
  
      // this assumes proper ordering
     for (int b = 0; b < 16; b += 2) {
          uint8_t spekChannel = 0x0F & (inData[b] >> spek_chan_shift);
          if (spekChannel < 7) {
            channel[spekChannel] = ((uint32_t)(inData[b] & spek_chan_mask) << 8) + inData[b+1];
  //          Serial.print(spekChannel);Serial.print(" ");Serial.println(channel[spekChannel]);
          }
       }
  //        Serial.println(" ");
     }
  } else {
    if (missCnt < 255)
      missCnt++; 
  }
} 

bool stale() {
  return missCnt > 25;    // missed 25 packets
}
int getAile(){
  return channel[1];
}
int getGear(){
  return channel[4];
}
int getAux(){
  return channel[5];
}
int getElev(){
  return channel[2];         
}
int getThro(){
  return channel[0];
}
int getRudd(){
  return channel[3];
}

private:
  // int channels[8];
  // int prevByte;
  // int channel;
  // bool cByte;

  int missCnt;
  int channel[5];
  
  int inData[16];
  int index;
  int inByte;
};


#endif /* SPECTRUMRECEIVER_H_ */

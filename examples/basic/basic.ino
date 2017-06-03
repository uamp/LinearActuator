#include <LinearActuator.h>

//LinearActuator(uint8_t pinA, uint8_t pinB, uint8_t pinCurrentSense); 
LinearActuator la(4,5,A0);



void setup(){
  Serial.begin(9600);
  pinMode(7,OUTPUT);   //current sense shutdown pin on TRV hardware
  digitalWrite(7,LOW);  //needs pulling low
  la.setThrowTime(10000,10000); //ms
  la.setStallCurrent(190); //mA
  la.calibrate(); //ends at 0
  delay(5000);
  la.setPosition(0); //shouldn't even twitch
  delay(5000);
  la.setPosition(255);
  delay(5000);
  la.setPosition(0);
}

void loop(){
  while (Serial.available() > 0) {
  
      // look for the next valid integer in the incoming serial stream:
      uint8_t demand_position = Serial.parseInt();
  
      if (Serial.read() == '\n') {
        Serial.println(demand_position);
        la.setPosition(demand_position);
      }
    }
    
}
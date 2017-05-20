#include <LinearActuator.h>

LinearActuator la(3,4,5,6);

void setup(){
  la.setThrowTime(10000); //ms
  la.setStallCurrent(100); //mA
  la.calibrate(); //ends at 0
  delay(3);
  la.setPosition(0); //shouldn't even twitch
  delay(10);
  la.setPosition(255);
  delay(10);
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
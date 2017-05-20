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

int main(){
}
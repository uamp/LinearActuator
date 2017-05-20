#include "Arduino.h"
#include "LinearActuator.h"


LinearActuator::LinearActuator(uint8_t pinA, uint8_t pinB, uint8_t pinEnable, uint8_t pinCurrentSense){ 	
	pin_A=pinA;
	pin_B=pinB;
	pin_enable=pinEnable;
	pin_current_sense=pinCurrentSense;
	pinMode(pin_A,OUTPUT);
	pinMode(pin_B,OUTPUT);
	pinMode(pin_enable,OUTPUT);
	pinMode(pin_current_sense,INPUT);
	motor_delay=500;
	throw_time=1000;
}

void LinearActuator::motorControl(bool motor_on, bool direction=true){
	digitalWrite(pin_enable,motor_on);
  	digitalWrite(pin_A,direction);
  	digitalWrite(pin_B,!direction);
}

uint16_t LinearActuator::readCurrent(){  
	return analogRead(pin_current_sense);
}


void LinearActuator::setPosition(uint8_t demanded_position){ // 0-255
	uint32_t time_start,travel_time;
	bool direction;
	int current_draw=0;

	if(demanded_position==current_position) {
		return;
	}
	if(demanded_position>current_position){
		direction=true;
		travel_time=(demanded_position-current_position)*throw_time/256;
	}
	if(demanded_position<current_position){
		direction=true;
		travel_time=(current_position-demanded_position)*throw_time/256;
	}
	if(demanded_position==0 || demanded_position==255) travel_time=throw_time+2000; //ensures it reaches the end stop from whereever it is

	time_start=millis();
	motorControl(true,direction); //motoring
	delay(motor_delay);

	do{
		current_draw=readCurrent();
	}while((current_draw < stall_current) && ((millis()-time_start)<travel_time)); //stop either at stall current or timeout (estimate of time needed for distance travel)

	motorControl(false);//stop motor
	current_position=demanded_position; //crude, refined below
	if(current_draw>=stall_current){ //update position if end-stop reached
		if(direction==true) current_position=255;  //determine which end
		if(direction==false) current_position=0;
	}
	
	
}

uint8_t LinearActuator::getPosition(){
	return current_position;
}

uint32_t LinearActuator::calibrate(){    //if using battery, as battery drop, throw time will change, therefore calibrate from time to time
	
	uint32_t time_start;
	time_start=millis();
	
	motorControl(true,true); //motoring out
	delay(motor_delay);
	do{
	}while(readCurrent() < stall_current && (millis()-time_start)<(2*throw_time));

	time_start=millis();
	motorControl(true,false); //motoring back in
	delay(motor_delay);
	do{
	}while(readCurrent() < stall_current && (millis()-time_start)<(2*throw_time));
	motorControl(false); //stop motor
	throw_time=(millis()-time_start);
	current_position=0;
	
	return throw_time;	

}

void LinearActuator::setStallCurrent(uint16_t stallCurrent){
	if(stallCurrent<1024) stall_current=stallCurrent;
}

void LinearActuator::setThrowTime(uint32_t throwTime){
	throw_time=throwTime;
}

uint32_t LinearActuator::getThrowTime(){
	return throw_time;
}


LinearActuator::~LinearActuator(){
}


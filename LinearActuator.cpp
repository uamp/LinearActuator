#include "Arduino.h"
#include "LinearActuator.h"


LinearActuator::LinearActuator(uint8_t _pin_A, uint8_t _pin_B, uint8_t _pin_enable, uint8_t _pin_current_sense){ 	
	pin_A=_pin_A;
	pin_B=_pin_B;
	pin_enable=_pin_enable;
	pin_current_sense=_pin_current_sense;
	pinMode(pin_A,OUTPUT);
	pinMode(pin_B,OUTPUT);
	pinMode(pin_enable,OUTPUT);
	pinMode(pin_current_sense,INPUT);
	motor_delay=500;
}

void LinearActuator::motorControl(bool motor_on, bool direction=true){
	digitalWrite(pin_enable,motor_on);
  	digitalWrite(pin_A,direction);
  	digitalWrite(pin_B,!direction);
}

uint16_t LinearActuator::readCurrent(){  
	return analogRead(pin_current_sense);
}


void LinearActuator::setPosition(uint8_t demanded_position){
	uint32_t time_start,travel_time;
	bool direction;

	if(demanded_position==current_position) {
		return;
	}
	if(demanded_position>current_position){
		direction=true;
		travel_time=(demanded_position-current_position)*throw_time/255;
	}
	if(demanded_position<current_position){
		direction=true;
		travel_time=(current_position-demanded_position)*throw_time/255;
	}

	time_start=millis();
	motorControl(true,direction); //motoring
	delay(motor_delay);
	do{
	}while(readCurrent() < stall_current && (millis()-time_start)<(travel_time));
	current_position=demanded_position; //crude, refined below
	if(readCurrent()>stall_current){ //update position if end-stop reached
		if(direction==true) current_position=255;
		if(direction==false) current_position=0;
	}
	motorControl(false);//stop motor
	
}

uint8_t LinearActuator::getPosition(){
	return current_position;
}

void LinearActuator::calibrate(){

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
	
}

void LinearActuator::setStallCurrent(uint16_t _stall_current){
	if(_stall_current<1024) stall_current=_stall_current;
}

void LinearActuator::setThrowTime(uint32_t _throw_time){
	throw_time=_throw_time;
}

LinearActuator::~LinearActuator(){
}


#include "Arduino.h"
#include "LinearActuator.h"
#include <math.h>


LinearActuator::LinearActuator(uint8_t pinA, uint8_t pinB, uint8_t pinCurrentSense){
	pin_A=pinA;
	pin_B=pinB;
	//pin_enable=pinEnable;
	pin_current_sense=pinCurrentSense;
	pinMode(pin_A,OUTPUT);
	pinMode(pin_B,OUTPUT);
	//pinMode(pin_enable,OUTPUT);
	pinMode(pin_current_sense,INPUT);
	motor_delay=1000; //delay after motor starts before first current reading (to allow it to overcome initial friction)
	// OUT: motoring out 0->100 direction true (throwTime1 & stallCurrent1)
	// IN: motoring in 100->0 direction false (throwTime2 & stallCurrent2)
	throw_time1=10000;
	throw_time2=10000;
	stall_current1=100;
	stall_current2=100;
	analogReference(INTERNAL);
}

void LinearActuator::motorControl(bool motor_on, bool direction=true){
	//if(pin_enable!=0) digitalWrite(pin_enable,motor_on);
	if(motor_on) {
		digitalWrite(pin_A,direction);
		digitalWrite(pin_B,!direction);
	} else {
		digitalWrite(pin_A,LOW);
		digitalWrite(pin_B,LOW);
	}
}

uint16_t LinearActuator::readCurrent(){
	uint16_t voltage, voltage1,voltage2,voltage3;
	uint16_t current;
	voltage1=analogRead(pin_current_sense);
	voltage2=analogRead(pin_current_sense);
	voltage=min(voltage1,voltage2);//ensure we have two consecutive readings the same
	current=voltage;  //*1000/1024;  //2.2k resistor means 1.1v per A.  Internal ref voltage is 1.1v.  Therefore 0-1023 is an 1Amp.
	return current;
}


void LinearActuator::setPosition(uint8_t demanded_position){ // demanded_position=0-255, returns +ve if out, -ve if in and value is final stall current
	uint32_t time_start,travel_time,stall_current;
	bool direction;
	int current_draw=0;

	if(demanded_position==current_position) {
		return;
	}
	if(demanded_position>current_position){
		direction=true;
		travel_time=(demanded_position-current_position)*throw_time1/256;
		stall_current=stall_current1;
	}
	if(demanded_position<current_position){
		direction=false;
		travel_time=(current_position-demanded_position)*throw_time2/256;
		stall_current=stall_current2;
	}
	if(demanded_position==0 || demanded_position==255) travel_time=travel_time+1500; //ensures it reaches the end stop from whereever it is

	time_start=millis();
	motorControl(true,direction); //motoring
	delay(motor_delay);

	do{
		current_draw=readCurrent();
	}while((current_draw < stall_current) && ((millis()-time_start)<travel_time)); //stop either at stall current or timeout (estimate of time needed for distance travel)

	if(current_draw>=stall_current) {
		last_throw_timeout=false;
	} else {
		last_throw_timeout=true;
	}

	motorControl(false);//stop motor
	current_position=demanded_position; //crude, refined below
	if(current_draw>=stall_current){ //update position if end-stop reached
		if(direction==true) current_position=255; 
		if(direction==false) current_position=0;
	}

	last_direction = direction;
	last_current = current_draw;
	last_throw_time = millis()-time_start;

	//return direction ? current_draw : -current_draw;

}

void LinearActuator::getThrowResults(bool & timeout, uint32_t & throwTime, int16_t & finalCurrent){ //final sign dependant on direction
    timeout=last_throw_timeout;
		throwTime=last_throw_time;
		if(last_direction){
			finalCurrent=  last_current;
		}else{
			finalCurrent= -last_current;
		}
}

uint8_t LinearActuator::getPosition(){
	return current_position;
}

void LinearActuator::calibrate(){    //if using battery, as battery drop, throw time will change, therefore calibrate from time to time

	uint32_t time_start;
	time_start=millis();
	// OUT: motoring out 0->100 direction true (throwTime1 & stallCurrent1)
	// IN: motoring in 100->0 direction false (throwTime2 & stallCurrent2)

	//reset motor position to be fully wound in
	motorControl(true,false); //motoring back
	delay(motor_delay);
	do{
	}while(readCurrent() < stall_current2 && (millis()-time_start)<(1.5*throw_time2));

	//wind out and measure throw_time1
	time_start=millis();
	motorControl(true,true); //motoring out
	delay(motor_delay);
	do{
	}while(readCurrent() < stall_current1 && (millis()-time_start)<(1.5*throw_time1));
	motorControl(false); //stop motor
	throw_time1=(millis()-time_start);

	//wind back in an measure throw_time2
	time_start=millis();
	motorControl(true,false); //motoring back in
	delay(motor_delay);
	do{
	}while(readCurrent() < stall_current2 && (millis()-time_start)<(1.5*throw_time2));
	motorControl(false); //stop motor
	throw_time2=(millis()-time_start);

	current_position=0;  //set position to zero now fully wound back in

}

void LinearActuator::setStallCurrent(uint16_t stallCurrent1, uint16_t stallCurrent2){
	if(stallCurrent1<1024) stall_current1=stallCurrent1;
	if(stallCurrent2<1024) stall_current2=stallCurrent2;
}

void LinearActuator::setThrowTime(uint32_t throwTime1, uint32_t throwTime2){
	throw_time1=throwTime1;
	throw_time2=throwTime2;
}

uint32_t LinearActuator::getThrowTime(uint8_t throwTimeSelect){
	if(throwTimeSelect==1) return throw_time1;
	if(throwTimeSelect==2) return throw_time2;
}

uint16_t LinearActuator::getStallCurrent(uint8_t stallCurrentSelect){
	if(stallCurrentSelect==1) return stall_current1;
	if(stallCurrentSelect==2) return stall_current2;

}

LinearActuator::~LinearActuator(){
}

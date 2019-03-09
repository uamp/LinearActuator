#include "Arduino.h"
#include "LinearActuator.h"
#include <math.h>


LinearActuator::LinearActuator(uint8_t pinA, uint8_t pinB, uint8_t pinCurrentSense, uint8_t pinCurrentSenseDisable){
	pin_A=pinA;
	pin_B=pinB;
	//pin_enable=pinEnable;
	pin_current_sense=pinCurrentSense;
	pin_current_sense_disable=pinCurrentSenseDisable;
	pinMode(pin_A,OUTPUT);
	digitalWrite(pin_A,LOW);
	pinMode(pin_B,OUTPUT);
	digitalWrite(pin_B,LOW);
	//pinMode(pin_enable,OUTPUT);
	pinMode(pin_current_sense,INPUT);
	pinMode(pin_current_sense_disable,OUTPUT);
	digitalWrite(pin_current_sense_disable,LOW);
	motor_delay=2000; //delay after motor starts before first current reading (to allow it to overcome initial friction)
	// OUT: motoring out 0->100 direction true (throwTime1 & stallCurrent1)
	// IN: motoring in 100->0 direction false (throwTime2 & stallCurrent2)
	throw_time1=10000;
	throw_time2=10000;
	stall_current1=100;
	stall_current2=100;
	analogReference(INTERNAL);

  //initialise last throw variables - just in case they are called early before a throw happens
	last_throw_time=0;
	last_current=0;
	last_throw_timeout=false;
	last_direction=false;
	current_position=50; //no idea at this point.  It should reset when it calibrates
}

void LinearActuator::motorControl(bool motor_on, bool direction=true){
	//if(pin_enable!=0) digitalWrite(pin_enable,motor_on);
	digitalWrite(pin_current_sense_disable,HIGH); // disable to reduce problems?
	delay(1);
	if(motor_on) {
		digitalWrite(pin_A,direction);
		digitalWrite(pin_B,!direction);
	} else {
		digitalWrite(pin_A,LOW);
		digitalWrite(pin_B,LOW);
	}
	delay(1);
	digitalWrite(pin_current_sense_disable,LOW); // re-enable
}

uint16_t LinearActuator::readCurrent(){
	//digitalWrite(pin_current_sense_disable,LOW); //enable current read
	uint16_t voltage, voltage1,voltage2,voltage3;
	uint16_t current;
	voltage1=analogRead(pin_current_sense);
	voltage2=analogRead(pin_current_sense);
	voltage=min(voltage1,voltage2);//ensure we have two consecutive readings the same
	current=voltage;  //*1000/1024;  //2.2k resistor means 1.1v per A.  Internal ref voltage is 1.1v.  Therefore 0-1023 is an 1Amp.
	//digitalWrite(pin_current_sense_disable,HIGH); //disable - idea is that this will stop the current spike perhaps
	return current;
}


void LinearActuator::setPosition(uint8_t demanded_position){ // demanded_position=0-100, returns +ve if out, -ve if in and value is final stall current
	uint32_t travel_time,stall_current;
	unsigned long time_start;
	bool direction;
	int current_draw=0;

	demanded_position=min(demanded_position,100);
	demanded_position=max(demanded_position,0);

	if(demanded_position==current_position) {
		return;
	} else if(demanded_position>current_position){
		direction=true;
		travel_time=((demanded_position-current_position) * throw_time1)/100;
		stall_current=stall_current1;
	} else { //(demanded_position<current_position)
		direction=false;
		travel_time=((current_position-demanded_position) * throw_time2)/100;
		stall_current=stall_current2;
	}
	if(demanded_position==0 || demanded_position==100) travel_time=travel_time+1000; //ensures it reaches the end stop from whereever it is
	time_start=millis();
	motorControl(true,direction); //motoring
	delay(static_cast<uint32_t>(0.9*travel_time));

	do{
		current_draw=readCurrent();
	}while((current_draw < stall_current) && ((millis()-time_start)<travel_time)); //stop either at stall current or timeout (estimate of time needed for distance travel)

	motorControl(false);//stop motor

	if(current_draw>=stall_current) {
		last_throw_timeout=false;
	} else {
		last_throw_timeout=true;
	}

	current_position=demanded_position; //crude, refined below
	if(current_draw>=stall_current){ //update position if end-stop reached
		if(direction==true) current_position=100;
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

	unsigned long time_start;
	time_start=millis();
	// OUT: motoring out 0->100 direction true (throwTime1 & stallCurrent1)
	// IN: motoring in 100->0 direction false (throwTime2 & stallCurrent2)

	//reset motor position to be fully wound in
	motorControl(true,false); //motoring back
	delay(motor_delay);
	do{
	}while(readCurrent() < stall_current2 && (millis()-time_start)<(1.2*throw_time2));

	//wind out and measure throw_time1
	time_start=millis();
	motorControl(true,true); //motoring out
	delay(throw_time1/2);
	do{
	}while(readCurrent() < stall_current1 && (millis()-time_start)<(1.2*throw_time1));
	motorControl(false); //stop motor
	throw_time1=millis()-time_start;

	//wind back in an measure throw_time2
	time_start=millis();
	motorControl(true,false); //motoring back in
	delay(throw_time2/2);
	do{
	}while(readCurrent() < stall_current2 && (millis()-time_start)<(1.2*throw_time2));
	motorControl(false); //stop motor
	throw_time2=millis()-time_start;

	current_position=0;  //set position to zero now fully wound back in

}

void LinearActuator::setStallCurrent(uint8_t stallCurrentSelect, uint16_t stallCurrent){
	if(stallCurrentSelect==1){
		if(stallCurrent<1024) stall_current1=stallCurrent;
	}
	if(stallCurrentSelect==2){
		if(stallCurrent<1024) stall_current2=stallCurrent;
	}
}

void LinearActuator::setStallCurrents(uint16_t stallCurrent1, uint16_t stallCurrent2){
	setStallCurrent(1,stallCurrent1);
	setStallCurrent(2,stallCurrent2);
}

void LinearActuator::setThrowTime(uint8_t throwTimeSelect, uint32_t throwTime){
	if(throwTimeSelect==1){
		throw_time1=throwTime;
	}
	if(throwTimeSelect==2){
		throw_time2=throwTime;
	}
}

void LinearActuator::setThrowTimes(uint32_t throwTime1, uint32_t throwTime2){
	setThrowTime(1,throwTime1);
	setThrowTime(2,throwTime2);
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

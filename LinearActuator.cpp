LinearActuator::LinearActuator(uint8_t _pin_A, uint8_t _pin_B, uint8_t _pin_enable, uint8_t _pin_current_sense); { 	
	pin_A=_pin_A;
	pin_B=_pin_B;
	pin_enable=_pin_enable;
	pin_current_sense=_pin_current_sense;
	pinMode(pin_A,OUTPUT);
	pinMode(pin_B,OUTPUT);
	pinMode(pin_enable,OUTPUT);
	pinMode(pin_current_sense,INPUT);
}

void LinearActuator::motorControl(bool motor_on, bool direction=true){
	digitalWrite(pin_enable,motor_on);
  	digitalWrite(pin_A,direction);
  	digitalWrite(pin_B,!direction);
}


hbridge::~hbridge(){
}


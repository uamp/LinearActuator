#ifndef __LA_H_INCLUDED__
#define __LA_H_INCLUDED__



class LinearActuator{
private:
	uint8_t pin_A;
	uint8_t pin_B;
	uint8_t pin_enable;
	uint8_t pin_current_sense;

	void motorControl(bool motor_on, bool direction);

public:
	LinearActuator(uint8_t _pin_A, uint8_t _pin_B, uint8_t _pin_enable, uint8_t _pin_current_sense); 
	~LinearActuator();

};

#endif // __LA_H_INCLUDED__ 	
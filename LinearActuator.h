#ifndef __LA_H_INCLUDED__
#define __LA_H_INCLUDED__



class LinearActuator{
private:
	uint8_t pin_A;
	uint8_t pin_B;
	uint8_t pin_enable;
	uint8_t pin_current_sense;
	
	uint16_t stall_current;
	uint8_t current_position;
	uint32_t throw_time;
	uint16_t motor_delay;
	
	void motorControl(bool motor_on, bool direction);
	uint16_t readCurrent();

public:
	LinearActuator(uint8_t pinA, uint8_t pinB, uint8_t pinEnable, uint8_t pinCurrentSense); 
	~LinearActuator();
	
	void setPosition(uint8_t demanded_position); //0 being one end, 255 being the other
	uint8_t getPosition();
	void calibrate();
	void setStallCurrent(uint16_t stallCurrent);
	void setThrowTime(uint32_t throwTime);

	
};

#endif // __LA_H_INCLUDED__ 	
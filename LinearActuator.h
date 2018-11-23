#ifndef __LA_H_INCLUDED__
#define __LA_H_INCLUDED__

// OUT: motoring out 0->100 direction true (throwTime1 & stallCurrent1)
// IN: motoring in 100->0 direction false (throwTime2 & stallCurrent2)

class LinearActuator{
private:
	uint8_t pin_A;
	uint8_t pin_B;
	//uint8_t pin_enable;
	uint8_t pin_current_sense;

	uint16_t stall_current1;
	uint16_t stall_current2;
	uint8_t current_position;
	uint32_t throw_time1; //when direction=true
	uint32_t throw_time2; //when direction=false
	uint16_t motor_delay;

	void motorControl(bool motor_on, bool direction);
	uint16_t readCurrent();

public:
	LinearActuator(uint8_t pinA, uint8_t pinB, uint8_t pinCurrentSense);
	~LinearActuator();

	void setPosition(uint8_t demanded_position); //0 being one end, 255 being the other
	uint8_t getPosition();
	void calibrate();
	void setStallCurrent(uint16_t stallCurrent1, uint16_t stallCurrent2); //out/in
	uint16_t getStallCurrent(uint8_t stallCurrentSelect); //1 or 2
	void setThrowTime(uint32_t throwTime1, uint32_t throwTime2); //out/in

};

#endif // __LA_H_INCLUDED__

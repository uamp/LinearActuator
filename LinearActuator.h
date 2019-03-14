#ifndef __LA_H_INCLUDED__
#define __LA_H_INCLUDED__

enum LAEepromAddress {LA_EEPROM_VERIFY=10,LA_EEPROM_TIME1=11, LA_EEPROM_TIME2=15, LA_EEPROM_CURRENT1=19, LA_EEPROM_CURRENT2=21, LA_EEPROM_POSITION=22};  //if this is changed, then you should change the verification code below
#define LA_EEPROM_VERIFY_CODE 101
//#define LA_WRITEPROTECT

// OUT: motoring out 0->100 direction true (throwTime1 & stallCurrent1)
// IN: motoring in 100->0 direction false (throwTime2 & stallCurrent2)

class LinearActuator{
private:
	uint8_t pin_A;
	uint8_t pin_B;
	//uint8_t pin_enable;
	uint8_t pin_current_sense;
	uint8_t pin_current_sense_disable;

	uint16_t stall_current1;
	uint16_t stall_current2;
	uint8_t current_position;
	uint32_t throw_time1; //when direction=true
	uint32_t throw_time2; //when direction=false
	uint16_t motor_delay;
	uint32_t last_throw_time;  //used to get results at the end of last travel
	uint16_t last_current; //used to get results at the end of last travel
	bool last_throw_timeout; //true if throwtime reached, false if stall current reached
	bool last_direction;


	void motorControl(bool motor_on, bool direction);
	uint16_t readCurrent();

public:
	LinearActuator(uint8_t pinA, uint8_t pinB, uint8_t pinCurrentSense, uint8_t pinCurrentSenseDisable);
	~LinearActuator();

	void setPosition(uint8_t demanded_position); //0 being one end, 100 being the other
	void setupPosition(uint8_t position); //'sets' position without moving it (after chip reset etc) 0 being one end, 100 being the other
	uint8_t getPosition();
	void getThrowResults(bool & timeout, uint32_t & throwTime, int16_t & finalCurrent); //final current sign dependant on direction
	void calibrate();
	void setStallCurrents(uint16_t stallCurrent1, uint16_t stallCurrent2); //out/in
	void setStallCurrent(uint8_t stallCurrentSelect, uint16_t stallCurrent); //1 or 2
	uint16_t getStallCurrent(uint8_t stallCurrentSelect); //1 or 2
	void setThrowTimes(uint32_t throwTime1, uint32_t throwTime2); //out/in
	void setThrowTime(uint8_t throwTimeSelect, uint32_t throwTime); //1 or 2
  uint32_t getThrowTime(uint8_t throwTimeSelect); //1 or 2

	bool loadEeprom(); //returns true if verification code is correct
	void setEeprom(); //set all variables to EEPROM
};

#endif // __LA_H_INCLUDED__

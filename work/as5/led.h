/*
 * led.h
 *
 *  Created on: Aug 1, 2018
 *      Author: eric
 *
 *      Initializes and controls BBG LEDs
 */

#ifndef LED_H_
#define LED_H_

enum Led_Mode {
	LED_MODE_BOUNCING,
	LED_MODE_BAR
};

void Led_init(void);
void Led_notifyTimerIsr(void);
void Led_doBackgroundWork(void);

uint32_t Led_getSpeed(void);
void Led_setSpeed(char newSpeed);

void Led_setMode(enum Led_Mode newMode);
void Led_toggleMode(void);

#endif /* LED_H_ */

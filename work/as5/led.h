/*
 * led.h
 *
 *  Created on: Aug 1, 2018
 *      Author: eric
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

#endif /* LED_H_ */

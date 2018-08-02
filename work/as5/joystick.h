/*
 * joystick.h
 *
 *  Created on: Aug 1, 2018
 *      Author: eric
 *
 *      Initializes and checks for joystick input
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

void Joystick_init(void);
void Joystick_notifyTimerIsr(void);
void Joystick_doBackgroundWork(void);

#endif /* JOYSTICK_H_ */

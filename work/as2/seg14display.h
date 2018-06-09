/*
 * seg14display.h
 *
 *  Created on: May 30, 2018
 *      Author: eric
 */

#include <stdbool.h>

#ifndef _SEG14DISPLAY_H_
#define _SEG14DISPLAY_H_

#define SEG14_DIGIT_0 0
#define SEG14_DIGIT_1 1

void Seg14_initialize();
void Seg14_shutdown();

// Turn the display on or off
void Seg14_setEnabled(int digitId, bool isEnabled);

// Set one of the characters to a specific digit
void Seg14_setDisplay(int digitId, char digit);

#endif /* _SEG14DISPLAY_H_ */

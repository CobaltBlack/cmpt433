// This module provides the interface to receive
// commands from the joystick

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#define JOYSTICK_NOOPT 0
#define JOYSTICK_UP 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define JOYSTICK_RIGHT 4
#define JOYSTICK_PUSH 5

void Joystick_init();
void Joystick_shutdown();

int Joystick_getInput();

#endif

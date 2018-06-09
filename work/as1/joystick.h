// This module provides the interface to receive
// commands from the joystick

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

typedef enum
{
	JOYSTICK_NOOPT,
	JOYSTICK_UP,
	JOYSTICK_DOWN,
	JOYSTICK_LEFT,
	JOYSTICK_RIGHT,
	JOYSTICK_PUSH,
} JoystickDirection;

void Joystick_init();
void Joystick_shutdown();

// Supports returning only one input direction at a time
JoystickDirection Joystick_getInput();

#endif

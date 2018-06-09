// This module provides the interface to control LEDs

#ifndef _LED_H_
#define _LED_H_

#define BBG_LED0 0
#define BBG_LED1 1
#define BBG_LED2 2
#define BBG_LED3 3

#define LED_OFF false
#define LED_ON true

void Led_init();
void Led_shutdown();

void Led_set(int ledId, bool isOn);
void Led_setAll(bool isOn);

void Led_flashAll(long onDuration_ns, long offDuration_ns);

#endif

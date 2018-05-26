#include <stdio.h>
#include "joystick.h"
#include "led.h"

int main()
{
	printf("Hello embedded world, from Eric Liu!\n");

	printf("\n");
	printf("Press the Zen cape's Joystick in the direction of the LED.\n");
 	printf("  UP for LED 0 (top)\n");
 	printf("  DOWN for LED 3 (bottom)\n");
 	printf("  LEFT/RIGHT for exit app. test test\n");

	// Main loop
//	while (currentGameNumber < numberOfGames) {
//
//	}

 	Led_test();

	return 0;
}

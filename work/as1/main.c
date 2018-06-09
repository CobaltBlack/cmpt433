#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fileutils.h"
#include "joystick.h"
#include "led.h"

static const int INCORRECT_LED_FLASH_NUM = 5;
static const long LED_FLASH_DURATION_NS = 100000000;

void initializeModules()
{
	srand(time(NULL));
	Led_init();
 	Joystick_init();
}

void shutdownModules()
{
	Led_shutdown();
 	Joystick_shutdown();
}

int main()
{
	initializeModules();

	printf("Hello embedded world, from Eric Liu!\n");

	printf("\n");
	printf("Press the Zen cape's Joystick in the direction of the LED.\n");
 	printf("  UP for LED 0 (top)\n");
 	printf("  DOWN for LED 3 (bottom)\n");
 	printf("  LEFT/RIGHT for exit app.\n");

 	// Initialize score
 	int currentScore = 0;
 	int numGamesPlayed = 0;

	// Main loop
 	while (true) {
		printf("Press joystick; current score (%d / %d)\n", currentScore, numGamesPlayed);

		// Randomly choose up or down
		bool bAnswerIsUp = (rand() % 2) == 0;

		// Enable corresponding LEDs
		if (bAnswerIsUp) {
			Led_set(BBG_LED0, LED_ON);
		} else {
			Led_set(BBG_LED3, LED_ON);
		}

		// Wait for joystick input (ignore JOYSTICK_PUSH)
		JoystickDirection joystickInput = JOYSTICK_NOOPT;
		while (joystickInput == JOYSTICK_NOOPT || joystickInput == JOYSTICK_PUSH) {
			joystickInput = Joystick_getInput();
		}

		// Handle joystick input
		Led_setAll(false);
		if (joystickInput == JOYSTICK_LEFT
			|| joystickInput == JOYSTICK_RIGHT)
		{
			// Exit game
			break;
		}
		else if ((bAnswerIsUp && joystickInput == JOYSTICK_UP)
				|| (!bAnswerIsUp && joystickInput == JOYSTICK_DOWN))
		{
			// Correct answer
			printf("Correct!\n");
			currentScore++;

			// Flash all LEDs once
			Led_flashAll(LED_FLASH_DURATION_NS, 0);
		}
		else
		{
			// Incorrect answer
			printf("Incorrect! :(\n");

			// Flash all LEDs 5 times
			for (int i = 0; i < INCORRECT_LED_FLASH_NUM; i++) {
				Led_flashAll(LED_FLASH_DURATION_NS, LED_FLASH_DURATION_NS);
			}
		}

		numGamesPlayed++;

		Led_setAll(false);

		// Wait for joystick no-opt
		while (joystickInput != JOYSTICK_NOOPT) {
			joystickInput = Joystick_getInput();
		}
 	}

 	printf("Your final score was (%d / %d)\n", currentScore, numGamesPlayed);
 	printf("Thank you for playing!\n");

 	shutdownModules();

	return 0;
}

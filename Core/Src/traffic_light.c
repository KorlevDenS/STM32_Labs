/*
 * traffic_light.c
 *
 *  Created on: Oct 22, 2024
 *      Author: Denis Korolev
 */

#include "traffic_light.h"


uint8_t checkButtonIsPressed() {
	GPIO_PinState buttonState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
	return buttonState == GPIO_PIN_RESET;
}

void toggleGreenLed(Is_turned_on state) {
	GPIO_PinState pinState = (GPIO_PinState) state;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, pinState);
}

void toggleRedYellowLed(Two_Color_Led_Mode mode, Is_turned_on state) {
	GPIO_PinState pinState = (GPIO_PinState) state;
	if (mode == RED_MODE) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, pinState & pinState);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, pinState & !pinState);
	} else if (mode == YELLOW_MODE) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, pinState & !pinState);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, pinState & pinState);
	}
}

void switchAndToggleLed(Traffic_color color, Is_turned_on state) {
	if (color == GREEN || color == GREEN_FLASHING) {
		toggleGreenLed(state);
	} else if (color == RED) {
		toggleRedYellowLed(RED_MODE, state);
	} else if (color == YELLOW) {
		toggleRedYellowLed(YELLOW_MODE, state);
	}
}

uint8_t tryToSwitchLight(Traffic_color from, Traffic_color to, uint32_t* last_change, uint32_t duration) {
	if (*last_change + duration <= HAL_GetTick()) {
		switchAndToggleLed(from, TURNED_OFF);
		switchAndToggleLed(to, TURNED_ON);
		*last_change = HAL_GetTick();
		return 1;
	}
	return 0;
}

void runTrafficLight(void)
{
	Traffic_light_state traffic_light[4] = {
			{RED, 12000},
			{GREEN, 3000},
			{GREEN_FLASHING, 3000},
			{YELLOW, 5000}
	};

	uint32_t now = HAL_GetTick();
	uint32_t* tick_start = &now;


	uint8_t pressed = 0;
	uint32_t change_moment = *tick_start;

	Is_turned_on current_light_state = TURNED_OFF;

	uint32_t last_flashing = *tick_start;
	switchAndToggleLed(RED, TURNED_ON);

	uint16_t i = 0;
	size_t arraySize = sizeof(traffic_light) / sizeof(Traffic_light_state);


	while (1)
	{
		/**
		 * If the button has been pressed and the next signal is red - switch to red 4X faster
		 * If not, then check if it's time to switch to the next color.
		 */
		if (pressed && traffic_light[i].Color == RED) {
			if (tryToSwitchLight(traffic_light[i].Color, traffic_light[(i + 1) % arraySize].Color, tick_start, traffic_light[i].Duration / 4)) {
				i = (i + 1) % arraySize;
				pressed = 0;
			}
		} else {
			if (tryToSwitchLight(traffic_light[i].Color, traffic_light[(i + 1) % arraySize].Color, tick_start, traffic_light[i].Duration)) {
				i = (i + 1) % arraySize;
			}
		}

		/**
		 * If current color is flashing green - flash if it is time to.
		 * Flashing is achieved by switching green led every FLASHING_INTERVAL
		 */
 		if (traffic_light[i].Color == GREEN_FLASHING && last_flashing + FLASHING_INTERVAL < HAL_GetTick()) {
			switchAndToggleLed(traffic_light[i].Color, current_light_state);
			current_light_state = !current_light_state;
			last_flashing = HAL_GetTick();
		}

		/**
		 * Checking if the button is pressed.
		 * State of the button is considered changed only if the change remains stable after UNSTABLE_PRESSING_PERIOD
		 */

		uint8_t current_button_state = checkButtonIsPressed();
		if (pressed != current_button_state && current_button_state == 1) {
			change_moment = HAL_GetTick();
			while (HAL_GetTick() - change_moment < UNSTABLE_PRESSING_PERIOD) {}
			if (pressed != current_button_state) {
				while(checkButtonIsPressed()){}
				pressed = current_button_state;
			}
		}

	}

}


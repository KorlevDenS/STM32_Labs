/*
 * traffic_light.h
 *
 *  Created on: Oct 22, 2024
 *      Author: Author: Denis Korolev
 */

#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define UNSTABLE_PRESSING_PERIOD 10
#define FLASHING_INTERVAL 200

typedef enum
{
	TURNED_OFF = 0,
	TURNED_ON
} Is_turned_on;

typedef enum
{
	RED_MODE = 0,
	YELLOW_MODE
} Two_Color_Led_Mode;

typedef enum
{
	RED = 0,
	GREEN,
	GREEN_FLASHING,
	YELLOW

} Traffic_color;

typedef struct
{
	Traffic_color Color;

	uint32_t Duration;

} Traffic_light_state;

void runTrafficLight(void);

#endif /* INC_TRAFFIC_LIGHT_H_ */

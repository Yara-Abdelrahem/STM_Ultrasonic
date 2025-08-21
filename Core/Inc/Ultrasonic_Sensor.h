#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

// Pin definitions
#define TRIG1_PORT GPIOA
#define TRIG1_PIN  GPIO_PIN_1
#define ECHO1_PORT GPIOA
#define ECHO1_PIN  GPIO_PIN_0

// #define TRIG2_PORT GPIOA
// #define TRIG2_PIN  GPIO_PIN_3
// #define ECHO2_PORT GPIOA
// #define ECHO2_PIN  GPIO_PIN_2

// #define TRIG3_PORT GPIOA
// #define TRIG3_PIN  GPIO_PIN_4
// #define ECHO3_PORT GPIOA
// #define ECHO3_PIN  GPIO_PIN_5

// #define TRIG4_PORT GPIOA
// #define TRIG4_PIN  GPIO_PIN_7
// #define ECHO4_PORT GPIOA
// #define ECHO4_PIN  GPIO_PIN_6

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim9;
extern UART_HandleTypeDef huart1;

// Declare global variables (extern only)
extern volatile uint32_t ic_val1, ic_val2;
extern volatile uint8_t is_first_captured;
extern volatile uint32_t distance;

// Function prototypes
void HCSR04_Trigger(void);

#endif

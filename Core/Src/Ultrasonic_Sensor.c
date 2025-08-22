#include "Ultrasonic_Sensor.h"

volatile uint32_t ic_val1 = 0, ic_val2 = 0;
volatile uint8_t is_first_captured = 0;
volatile uint32_t distance = 0;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim9;
extern UART_HandleTypeDef huart1;

const Ultrasonic us[4] = {
    {TRIG1_PORT, TRIG1_PIN, ECHO1_PORT, ECHO1_PIN, &htim5},
    {TRIG2_PORT, TRIG2_PIN, ECHO2_PORT, ECHO2_PIN, &htim9},
    {TRIG3_PORT, TRIG3_PIN, ECHO3_PORT, ECHO3_PIN, &htim2},
    {TRIG4_PORT, TRIG4_PIN, ECHO4_PORT, ECHO4_PIN, &htim3},
};


uint16_t US_Measure_cm(const Ultrasonic *s, uint8_t index)
{
    uint32_t start, stop;
    uint32_t tick_start;

    // Ensure TRIG low
    HAL_GPIO_WritePin(s->trig_port, s->trig_pin, GPIO_PIN_RESET);
    HAL_Delay(2);

    // Send 10 µs pulse
    HAL_GPIO_WritePin(s->trig_port, s->trig_pin, GPIO_PIN_SET);
    __HAL_TIM_SET_COUNTER(s->htim, 0); // reset counter
    while (__HAL_TIM_GET_COUNTER(s->htim) < 10)
        ; // wait 10 µs
    HAL_GPIO_WritePin(s->trig_port, s->trig_pin, GPIO_PIN_RESET);

    // Wait for ECHO high
    tick_start = HAL_GetTick();
    while (!HAL_GPIO_ReadPin(s->echo_port, s->echo_pin))
    {
        if (HAL_GetTick() - tick_start > 50) // 50 ms timeout
            return 0;                        // no echo
    }
    start = __HAL_TIM_GET_COUNTER(s->htim);

    // Wait for ECHO low
    tick_start = HAL_GetTick();
    while (HAL_GPIO_ReadPin(s->echo_port, s->echo_pin))
    {
        if (HAL_GetTick() - tick_start > 50) // 50 ms timeout
            return 0;                        // stuck high
    }
    stop = __HAL_TIM_GET_COUNTER(s->htim);

    // Calculate pulse width (µs)

    uint32_t pulse_us = (stop >= start) ? (stop - start) : (0xFFFFFFFFu - start + stop + 1u);

    // Convert to cm: distance = (pulse_us * 0.0343) / 2
    uint32_t cm = pulse_us / 58; // HC-SR04 formula: 58 us = 1 cm

    if (cm > 400)
        cm = 400; // max range ~4m

    return (uint16_t)cm;
}

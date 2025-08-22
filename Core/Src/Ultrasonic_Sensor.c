#include "Ultrasonic_Sensor.h"

volatile uint32_t ic_val1 = 0, ic_val2 = 0;
volatile uint8_t is_first_captured = 0;
volatile uint32_t distance = 0;

const Ultrasonic us[4] = {
  {TRIG1_PORT, TRIG1_PIN, ECHO1_PORT, ECHO1_PIN},
  {TRIG2_PORT, TRIG2_PIN, ECHO2_PORT, ECHO2_PIN},
  {TRIG3_PORT, TRIG3_PIN, ECHO3_PORT, ECHO3_PIN},
  {TRIG4_PORT, TRIG4_PIN, ECHO4_PORT, ECHO4_PIN},
};


uint16_t US_Measure_cm(const Ultrasonic *s)
{
  uint32_t start, stop;
  uint32_t tick_start;

  // Ensure TRIG low
  HAL_GPIO_WritePin(s->trig_port, s->trig_pin, GPIO_PIN_RESET);
  HAL_Delay(2);

  // Send 10 us pulse
  HAL_GPIO_WritePin(s->trig_port, s->trig_pin, GPIO_PIN_SET);
  __HAL_TIM_SET_COUNTER(&htim5, 0);
  while (__HAL_TIM_GET_COUNTER(&htim5) < 10);
  HAL_GPIO_WritePin(s->trig_port, s->trig_pin, GPIO_PIN_RESET);

  // Wait for ECHO high
  tick_start = HAL_GetTick();
  while (!HAL_GPIO_ReadPin(s->echo_port, s->echo_pin)) {
    if (HAL_GetTick() - tick_start > 10) return 0;
  }
  start = __HAL_TIM_GET_COUNTER(&htim5);

  // Wait for ECHO low
  tick_start = HAL_GetTick();
  while (HAL_GPIO_ReadPin(s->echo_port, s->echo_pin)) {
    if (HAL_GetTick() - tick_start > 50) return 0;
  }
  stop = __HAL_TIM_GET_COUNTER(&htim5);

  // Calculate pulse width (us)
  uint32_t pulse_us = (stop >= start) ? (stop - start)
                                      : (0xFFFFFFFFu - start + stop + 1u);

  // Distance cm = (pulse_us * 0.0343) / 2
  uint32_t cm = (pulse_us * 34u) / 2000u;
  if (cm > 400) cm = 400;

  return (uint16_t)cm;
}



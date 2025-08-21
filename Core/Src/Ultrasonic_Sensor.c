#include "Ultrasonic_Sensor.h"

volatile uint32_t ic_val1 = 0, ic_val2 = 0;
volatile uint8_t is_first_captured = 0;
volatile uint32_t distance = 0;

void HCSR04_Trigger(void)
{
    HAL_GPIO_WritePin(TRIG1_PORT, TRIG1_PIN, GPIO_PIN_SET);
    HAL_Delay(0.01); // 10 us pulse
    HAL_GPIO_WritePin(TRIG1_PORT, TRIG1_PIN, GPIO_PIN_RESET);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    char distance_msg[64];
    snprintf(distance_msg, sizeof(distance_msg), " Distance = %04X\r\n", distance);
    HAL_UART_Transmit(&huart1, (uint8_t *)distance_msg, strlen(distance_msg), HAL_MAX_DELAY);
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        if (!is_first_captured)
        {
            ic_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            is_first_captured = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else
        {
            ic_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            __HAL_TIM_SET_COUNTER(htim, 0);

            uint32_t pulse_length = (ic_val2 > ic_val1) ? (ic_val2 - ic_val1) : ((htim->Instance->ARR - ic_val1) + ic_val2);

            distance = pulse_length / 58; // cm

            char distance_msg[64];
            snprintf(distance_msg, sizeof(distance_msg), " Distance = %04X\r\n", distance);
            HAL_UART_Transmit(&huart1, (uint8_t *)distance_msg, strlen(distance_msg), HAL_MAX_DELAY);
            is_first_captured = 0;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);

            //  Check distance and control LED on 
            
            if (distance > 0 && distance < 20) // Example threshold for triggering an action
            {                                                       // e.g. < 20 cm
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // LED ON
                // HAL_Delay(1000);
                // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // LED OFF
            }
            else
            {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // LED OFF
            }
            HAL_Delay(1000);
        }
    }
}

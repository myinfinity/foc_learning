#include "HardWare_Driver.h"

/*
 * @brief 电机硬件初始化
 * @param  无
 * @retval 无
 */
void Motor_Hardware_Init(void)
{
    /* 初始化定时器TIM1：通道4 */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    /* 使能定时器1：通道4捕获中断 */
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC4);
    /* 初始化6路PWM */
    Motor_EnablePWM();
}

/*
 * @brief 使能电机PWM输出
 * @param  无
 * @retval 无
 */
void Motor_EnablePWM(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); 
}

/*
 * @brief 禁止电机PWM输出
 * @param  无
 * @retval 无
 */
void Motor_DisablePWM(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3); 
}

/*
 * @brief 设置电机PWM比较值
 * @param compare1: PWM通道1的占空比
 * @param compare2: PWM通道2的占空比
 * @param compare3: PWM通道3的占空比
 * @retval 无
 */
void Motor_SetPWM(uint16_t compare1, uint16_t compare2, uint16_t compare3)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, compare1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, compare2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, compare3);
}

/*
 * @brief 读取MT6701编码器数据
 * @param  无
 * @retval 编码器数据
 */
uint16_t MT6701_ReadData(void)
{
    uint8_t rx[3] = {0};
    uint8_t tx[3] = {0};

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx , rx, 3, 0xffffff);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    return ((rx[0] << 6) | (rx[1] >> 2)) & 0x3fff;
}

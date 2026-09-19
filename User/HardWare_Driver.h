#ifndef HARDWARE_DRIVER_H
#define HARDWARE_DRIVER_H

#include "tim.h"
#include "stdint.h"
#include "spi.h"

/*================函数声明==============*/
void Motor_Hardware_Init(void);
void Motor_EnablePWM(void);
void Motor_DisablePWM(void);
void Motor_SetPWM(uint16_t compare1, uint16_t compare2, uint16_t compare3);
uint16_t MT6701_ReadData(void);

#endif // HARDWARE_DRIVER_H

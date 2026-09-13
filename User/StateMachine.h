#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "FOCAlgorithm.h"
#include "FOCAlgorithm.h"
#include "Hardware_Driver.h"
#include "Motor_Config.h"
/*
 * @brief: 电机状态机结构体
 * 
 */
typedef struct {
    FOC_TypeDef                     FOC;    /*FOC状态*/
    VF_Control_Speed_TypeDef        VF;     /*VF速度控制状态*/
} Motor_TypeDef;

void Motor_StateMachine_Init(Motor_TypeDef *MotorSystem);
void Motor_StateMachine_Run(Motor_TypeDef *MotorSystem);

/*
* @brief: 电机状态机结构体变量
* 
*/
extern Motor_TypeDef MotorSystem;


#endif // STATE_MACHINE_H

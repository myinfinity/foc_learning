#include "StateMachine.h"

/*
 * @brief: 创建状态机结构体变量

 */
Motor_TypeDef MotorSystem;

/*
 * @brief: 初始化电机状态机
 * @param: MotorSystem - 电机状态机结构体指针
 * @return: None
 */
void Motor_StateMachine_Init(Motor_TypeDef *MotorSystem)
{
    /* 初始化FOC参数 */
   Motor_Hardware_Init();
  /* 设置PWM周期计数值和直流母线电压 */
  Set_Udc_Tpwm_parameters(&MotorSystem->FOC, UDC, TPWM);
}

/*
 * @brief: 运行电机状态机
 * @param: MotorSystem - 电机状态机结构体指针
 * @return: None
 */
void Motor_StateMachine_Run(Motor_TypeDef *state)
{
    /*创建虚拟角度*/
    static float Theta = 0.0f; 
    /*虚拟角度累加*/
    Theta += 0.0003f; 
    /*限制角度范围*/
    if (Theta >= _2PI){ 
        Theta -= _2PI;
    }

    /*设置电机角度*/
    state->FOC.Theta = Theta;

    /*设置d轴电压和q轴电压*/
    state->FOC.Vd = 0.0f;
    state->FOC.Vq = 0.5f;

    Foc_VoltageUpdate(&state->FOC);

    /*设置PWM比较值*/
    Motor_SetPWM(state->FOC.Tcmp1, state->FOC.Tcmp2, state->FOC.Tcmp3);
}

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
    /* 初始化VF速度控制参数 */
    VF_SpeedControl_Param_Init(&MotorSystem->VF, 50, 0, 0.0001f, FOC_TS);
    /*编码器参数初始化 */
    Encode_Param_Init(&MotorSystem->Encoder,ENCODER_LINE,0);

}


/*
 * @brief: 运行电机状态机
 * @param: MotorSystem - 电机状态机结构体指针
 * @return: None
 */
void Motor_StateMachine_Run(Motor_TypeDef *state)
{
    uint16_t data = 0;
    /* 读取编码器角度 */
    data = MT6701_ReadData();

    /* 执行编码器更新函数 */
    Encoder_Update(&state->Encoder, data);
    /* 设置电机角度 */
    VF_SpeedControl_Update(&state->VF, &state->FOC.Theta);
    
    /*设置d轴电压和q轴电压*/
    //state->FOC.Vd = 0.0f;
    //state->FOC.Vq = 0.5f;

    Foc_VoltageUpdate(&state->FOC);

    /*设置PWM比较值*/
    Motor_SetPWM(state->FOC.Tcmp1, state->FOC.Tcmp2, state->FOC.Tcmp3);
}

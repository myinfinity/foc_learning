#ifndef FOCALGORITHM_H
#define FOCALGORITHM_H
#include "math.h"


#define _2PI 6.283185307179f
/*
 * @brief: FOC参数结构体
 *
 */
typedef struct
{
    float UDC;      /* 直流母线电压 */
    float Tpwm;     /* PWM周期计数值 */
    float Vd;       /* d轴电压 */
    float Vq;       /* q轴电压 */
    float Theta;    /* 电机角度 */

    float Valpha;   /* α轴电压 */
    float Vbeta;    /* β轴电压 */

    float Vu;       /* u相电压 */
    float Vv;       /* v相电压 */
    float Vw;       /* w相电压 */

    float Vu_Mod;   /* u相调制电压 */
    float Vv_Mod;   /* v相调制电压 */
    float Vw_Mod;   /* w相调制电压 */

    int Tcmp1;     /* U相的PWM比较值 */
    int Tcmp2;     /* V相的PWM比较值 */
    int Tcmp3;     /* W相的PWM比较值 */
} FOC_TypeDef;

/*
 * @brief: VF角度生成器参数结构体
*/
typedef struct
{
    /* 输入参数 */
    float Acc;                  /* 加速度 */
    float Target_Speed;         /* 目标速度 */
    float Dead_Zone;            /* 死区阈值 */
    float Ts;                  /* 采样周期 */

    /* 中间参数 */
    float Speed_Error;          /* 速度误差 */
    float Speed_Error_Dead;     /* 死区处理后的速度误差 */
    float Acc_Dir;              /* 加速度方向 */
    float Speed_Integrator;     /* 速度积分器 */
    float Theta_Integrator;     /* 角度积分器 */

    /* 输出参数 */
    float Speed;                /* 实际速度 */
    float Theta;                /* 实际角度 */
} VF_Control_Speed_TypeDef;

/*======== 函数声明 ========*/
void Rev_Park_Transf(FOC_TypeDef *state);
void Rev_Clark_Transf(FOC_TypeDef *state);
void SVPWM_ZeroSqlInject(FOC_TypeDef *state);
void Set_Udc_Tpwm_parameters(FOC_TypeDef *state, float Udc, float Tpwm);
void Foc_VoltageUpdate(FOC_TypeDef *state);
void VF_SpeedControl_Param_Init(VF_Control_Speed_TypeDef *vf, float Acc, float Target_Speed, float Dead_Zone, float Ts);
void VF_SpeedControl_Update(VF_Control_Speed_TypeDef *vf, float *Target_Theta_Add);

#endif // FOCALGORITHM_H

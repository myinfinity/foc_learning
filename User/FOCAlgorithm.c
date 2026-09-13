#include "FOCAlgorithm.h"

/*
 * @brief: 反Park变换
 * @param: foc - FOC参数结构体指针
 * @return: None
 */
void Rev_Park_Transf(FOC_TypeDef *state)
{
    /* 反变换公式 */
    state->Valpha = state->Vd * cosf(state->Theta) - state->Vq * sinf(state->Theta);
    state->Vbeta  = state->Vd * sinf(state->Theta) + state->Vq * cosf(state->Theta);
}

/*
 * @brief: 反Clark变换
 * @param: foc - FOC参数结构体指针
 * @return: None
 */
void Rev_Clark_Transf(FOC_TypeDef *state)
{
    /* 反变换公式 */
    state->Vu = state->Valpha;
    state->Vv = -0.5f * state->Valpha + 0.8660254f * state->Vbeta; // 0.86602540378 = sqrt(3)/2
    state->Vw = -0.5f * state->Valpha - 0.8660254f * state->Vbeta;
}

/*
 * @brief: SVPWM零序注入
 * @param: foc - FOC参数结构体指针
 * @return: None
 */
void SVPWM_ZeroSqlInject(FOC_TypeDef *state)
{
    /* 计算三相最大 */
    float Vmax = (state->Vu > state->Vv) ? (state->Vu > state->Vw ? state->Vu : state->Vw) : (state->Vv > state->Vw ? state->Vv : state->Vw);
    /* 计算三相最小 */
    float Vmin = (state->Vu < state->Vv) ? (state->Vu < state->Vw ? state->Vu : state->Vw) : (state->Vv < state->Vw ? state->Vv : state->Vw);
    
    float V0 = -0.5f*(Vmax + Vmin) ;

    /*计算三相马鞍波电压*/
    state->Vu_Mod = state->Vu + V0;
    state->Vv_Mod = state->Vv + V0;
    state->Vw_Mod = state->Vw + V0;

    /*将三相马鞍波电压转换为PWM比较值*/
    state->Tcmp1 = ((state->Vu_Mod  /state->UDC) + 0.5) *state->Tpwm;
    state->Tcmp2 = ((state->Vv_Mod  /state->UDC) + 0.5) *state->Tpwm;
    state->Tcmp3 = ((state->Vw_Mod  /state->UDC) + 0.5) *state->Tpwm;
}

/*
 * @brief: 设置直流母线电压和PWM周期参数
 * @param: state - FOC参数结构体指针
 * @param: Udc - 直流母线电压
 * @param: Tpwm - PWM周期计数值
 * @return: None
 */
void Set_Udc_Tpwm_parameters(FOC_TypeDef *state, float Udc, float Tpwm)
{
    state->UDC = Udc;
    state->Tpwm = Tpwm;
}

/**
 * @brief: FOC电压更新函数
 * @param: state - FOC参数结构体指针
 * @return: None
 */
void Foc_VoltageUpdate(FOC_TypeDef *state)
{
    /*反Park变换*/
    Rev_Park_Transf(state);

    /*反Clarke变换*/
    Rev_Clark_Transf(state);

    /*SVPWM*/
    SVPWM_ZeroSqlInject(state);
}

/*
 * @brief: VF速度控制参数初始化
 * @param: vf - VF速度控制参数结构体指针
 * @param: Acc - 加速度
 * @param: Target_Speed - 目标速度
 * @param: Dead_Zone - 死区阈值
 * @param: Ts - 采样周期
 * @return: None
 */
void VF_SpeedControl_Param_Init(VF_Control_Speed_TypeDef *vf, float Acc, float Target_Speed, float Dead_Zone, float Ts)
{
    vf->Acc = Acc;
    vf->Target_Speed = Target_Speed;
    vf->Dead_Zone = Dead_Zone;
    vf->Ts = Ts;
}

/*
 * @brief: VF速度控制更新函数
 * @param: vf - VF速度控制参数结构体指针
 * @param: Target_Theta_Add - 目标角度增量指针
 * @return: None
*/
void VF_SpeedControl_Update(VF_Control_Speed_TypeDef *vf, float *Target_Theta_Add)
{
    /* 计算速度误差 */
    vf->Speed_Error = vf->Target_Speed - vf->Speed;
    
    /*死区处理*/
    if (fabs(vf->Speed_Error) < vf->Dead_Zone)
    {
        vf->Speed_Error_Dead = 0;
    }
    else
    {
        vf->Speed_Error_Dead = vf->Speed_Error;
    }

    /* 判断加速度方向 */ 
    if (vf->Speed_Error_Dead > 0)
    {
        vf->Acc_Dir = 1;
    }
    else if (vf->Speed_Error_Dead < 0)
    {
        vf->Acc_Dir = -1;
    }
    else
    {
        vf->Acc_Dir = 0;
    }

    /* 速度积分器 */
    vf->Speed_Integrator += vf->Acc * vf->Acc_Dir * vf->Ts;

    /* 角度积分器 */
    vf->Theta_Integrator += vf->Speed_Integrator * vf->Ts;

    /* 角度取模*/
    vf->Theta = fmod(vf->Theta_Integrator, _2PI);
    /* 确保角度为正 */
    if (vf->Theta < 0)
    {
        vf->Theta += _2PI;
    }

    /* 输出实际速度 */
    vf->Speed = vf->Speed_Integrator;

    /* 输出目标角度增量 */
    *Target_Theta_Add = vf->Theta;
}


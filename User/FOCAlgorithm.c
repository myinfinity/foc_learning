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

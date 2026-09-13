#include "FOCAlgorithm.h"

/*创建FOC结构体变量*/
FOC_TypeDef FOC;

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
    
    float V0 = -0.5*(Vmax + Vmin) ;

    /*计算三相马鞍波电压*/
    state->Vu_Mod = state->Vu + V0;
    state->Vv_Mod = state->Vv + V0;
    state->Vw_Mod = state->Vw + V0;
}


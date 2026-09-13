#ifndef FOCALGORITHM_H
#define FOCALGORITHM_H
#include "math.h"
/*
 * @brief: FOC参数结构体
 *
 */
typedef struct
{
    float Vd;       /* d轴电压 */
    float Vq;       /* q轴电压 */
    float Theta;   /* 电机角度 */

    float Valpha;   /* α轴电压 */
    float Vbeta;    /* β轴电压 */

    float Vu;       /* u相电压 */
    float Vv;       /* v相电压 */
    float Vw;       /* w相电压 */

    float Vu_Mod;   /* u相调制电压 */
    float Vv_Mod;   /* v相调制电压 */
    float Vw_Mod;   /* w相调制电压 */


} FOC_TypeDef;

/*======== 函数声明 ========*/
void Rev_Park_Transf(FOC_TypeDef *state);
void Rev_Clark_Transf(FOC_TypeDef *state);
void SVPWM_ZeroSqlInject(FOC_TypeDef *state);

/*======== 全局变量声明 ========*/
extern FOC_TypeDef FOC;

#endif // FOCALGORITHM_H

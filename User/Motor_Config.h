#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

/*=============系统时序常量============*/
#define FOC_HZ      20000                           /* FOC中断频率 20kHz */
#define FOC_TS      (1 / (float) FOC_HZ)            /* 采样周期 50us */

/*=============数学常量=============*/
#define _2PI 6.283185307179f

/*=============SVPWM常量=============*/
#define UDC 12.0f           //直流母线电压
#define TPWM 4249.0f        //PWM周期计数值


#endif // MOTOR_CONFIG_H

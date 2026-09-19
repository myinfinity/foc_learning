#ifndef ENCODER_H
#define ENCODER_H

#include "stdint.h"

#define _2PI 6.283185307179f

/*
 * @brief   编码器结构体
 */
typedef struct {
    uint32_t Line;          /*编码器线数*/
    uint8_t  Dir;           /*编码器方向*/
    uint32_t Pulse_Data;    /*编码器原始数据*/
    float    Theta;         /*机械角度*/
}Encoder_Typedef;

/*
 *  @brief   编码器参数初始化
 */
void Encode_Param_Init(Encoder_Typedef *state, uint32_t line, uint8_t dir);
void Encoder_Update(Encoder_Typedef *state, uint32_t Data);

#endif

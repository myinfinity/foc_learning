#include "encoder.h"

/*
 * @brief  初始化编码器参数
 * @param  state: 编码器结构体指针
 * @param  line: 编码器线数
 * @param  dir: 编码器方向（0：同向，1：反向）
 */
void Encode_Param_Init(Encoder_Typedef *state, uint32_t line, uint8_t dir)
{
    state->Line = line;
    state->Dir = dir; 
}

/*
 * @brief  编码器角度更新函数
 * @param  state: 编码器结构体指针
 * @param  Data: 读取到的编码器数据
 */
void Encoder_Update(Encoder_Typedef *state, uint32_t Data)
{
    /* 判断编码器方向 */
    if (state->Dir == 0)
    {
        state->Pulse_Data = Data;
    }else
    {
        state->Pulse_Data = state->Line - Data;
    }

    /* 计算编码器角度 */
    state->Theta = state->Pulse_Data / (float)state->Line *_2PI;
}

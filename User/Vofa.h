#ifndef _VOFA_H
#define _VOFA_H

#include "stdint.h"
#include "string.h"
#include "usart.h"
#include "StateMachine.h"
/*====宏定义====*/
#define DATA_NUM 9                  /*定义发送数量*/
#define DATA_LEN (DATA_NUM*4 + 4)   /*定义发送长度*/

/*====结构体定义====*/
typedef struct Vofa{
    float Send_Data_Arrayt[DATA_NUM]; /*定义发送数据数组*/
    uint8_t Byte_Data_Array[DATA_LEN];        /*定义字节发送数据*/ 
}VofaSend_Type;

/*====变量定义====*/
extern VofaSend_Type VofaSend;     /*定义VofaSend_Type结构体变量*/

/*====函数定义====*/
void Vofa_Send_Task(void);      /*定义发送函数*/
void Vofa_Data_Process(void);   /*定义数据处理函数*/

#endif

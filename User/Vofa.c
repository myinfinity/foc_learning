#include "Vofa.h"

VofaSend_Type VofaSend;


/*
 * @brief  Vofa发送任务
 * @param  None
 * @retval None
 */
void Vofa_Send_Task(void){
    /*填入需要发送的数据*/
    VofaSend.Send_Data_Arrayt[0] = MotorSystem.FOC.Theta;
    VofaSend.Send_Data_Arrayt[1] = MotorSystem.VF.Target_Speed;
    VofaSend.Send_Data_Arrayt[2] = MotorSystem.VF.Speed;
    VofaSend.Send_Data_Arrayt[3] = MotorSystem.VF.Theta;
	
    VofaSend.Send_Data_Arrayt[4] = MotorSystem.FOC.Vv;
    VofaSend.Send_Data_Arrayt[5] = MotorSystem.FOC.Vw;
    VofaSend.Send_Data_Arrayt[6] = MotorSystem.Encoder.Pulse_Data;
    VofaSend.Send_Data_Arrayt[7] = MotorSystem.Encoder.Theta;
    VofaSend.Send_Data_Arrayt[8] = 0;

    /*数据处理*/
    Vofa_Data_Process();
}

/*
* @brief  Vofa数据处理函数
* @param  None
* @retval None
*/
void Vofa_Data_Process(void){
    /*通过拷贝将浮点数据转换为单字节数据*/
    memcpy(VofaSend.Byte_Data_Array, VofaSend.Send_Data_Arrayt, DATA_LEN - 4);

    /*填补通信协议的结尾*/
    VofaSend.Byte_Data_Array[DATA_LEN - 4] = 0x00;
    VofaSend.Byte_Data_Array[DATA_LEN - 3] = 0x00;
    VofaSend.Byte_Data_Array[DATA_LEN - 2] = 0x80;
    VofaSend.Byte_Data_Array[DATA_LEN - 1] = 0x7f;

    /*发送串口数据*/
    HAL_UART_Transmit(&huart3,VofaSend.Byte_Data_Array,DATA_LEN,HAL_MAX_DELAY);
}

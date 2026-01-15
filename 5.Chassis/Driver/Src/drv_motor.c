/*
 *  Project      :  
 *  
 *  FilePath     : drv_motor.c
 *  Description  : 这个文件是控制电机相关的驱动
 *  LastEditors  : 李宪
 *  Date         : 2024年8月19日19:18:51
 *  LastEditTime : 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drv_motor.h"
#include "bsp_can.h"




const uint32_t Const_Motor_MOTOR_OFFLINE_TIME = 200;		//电机离线时间的常量值，单位毫秒
const uint32_t Const_Motor_MOTOR_TX_EXTID = 0x01;				//电机发送数据的扩展ID，0x01。
const uint32_t Const_Motor_MOTOR_TX_DLC = 8;						//数据长度，8字节
const uint32_t Const_Motor_MOTOR_RX_DLC = 8;

/*	创建控制各组电机的结构体指针	*/
Motor_MotorGroupTypeDef *Motor_groupHandle[MOTOR_GROUP_NUM];

/*	创建各个组电机的结构体	*/
Motor_MotorGroupTypeDef Motor_ChassisMotors;
Motor_MotorGroupTypeDef Motor_PitchMotors;        
Motor_MotorGroupTypeDef Motor_ShootMotors;

/*	创建各个电机的结构体	*/
Motor_MotorTypeDef Motor_PitchMotor;
Motor_MotorTypeDef Motor_YawMotor;          //yaw
Motor_MotorTypeDef Motor_ShootLeftMotor;
Motor_MotorTypeDef Motor_ShootRightMotor;
Motor_MotorTypeDef Motor_FeedMotor;

/*	创建底盘四个电机的结构体	*/
Motor_MotorTypeDef Motor_ChassisFontRightMotor;
Motor_MotorTypeDef Motor_ChassisFontLeftMotor;
Motor_MotorTypeDef Motor_ChassisBackLeftMotor;
Motor_MotorTypeDef Motor_ChassisBackRightMotor;


/**
	* @brief      初始化各个组中的电机
  * @param      NULL
  * @retval     使用本函数需要包含一下“can.h”
  */
void Motor_InitAllMotors() {
    Motor_groupHandle[0] = &Motor_PitchMotors;
    Motor_InitMotorGroup(&Motor_PitchMotors, Motor_TYPE_RM6020, 2, &hcan1, 0x1FF); 
		Motor_InitMotor(&Motor_YawMotor,Motor_TYPE_RM6020,0x205,0.1,gm6020_encoder_callback);
    Motor_InitMotor(&Motor_PitchMotor, Motor_TYPE_RM6020, 0x206, 0.1, gm6020_encoder_callback);
	  Motor_PitchMotors.motor_handle[0] = &Motor_YawMotor;
    Motor_PitchMotors.motor_handle[1] = &Motor_PitchMotor;
    
    Motor_groupHandle[1] = &Motor_ShootMotors;
    Motor_InitMotorGroup(&Motor_ShootMotors, Motor_TYPE_RM3508, 3, &hcan2, 0x200);
    Motor_InitMotor(&Motor_ShootRightMotor, Motor_TYPE_RM3508, 0x201, 0.1, rm3508_encoder_callback);
    Motor_InitMotor(&Motor_ShootLeftMotor, Motor_TYPE_RM3508, 0x202, 0.1, rm3508_encoder_callback);
    Motor_InitMotor(&Motor_FeedMotor, Motor_TYPE_RM2006, 0X203, 0.1, rm2006_encoder_callback);
	  Motor_ShootMotors.motor_handle[0] = &Motor_ShootRightMotor;
    Motor_ShootMotors.motor_handle[1] = &Motor_ShootLeftMotor;
    Motor_ShootMotors.motor_handle[2] = &Motor_FeedMotor;	  
	
		Motor_groupHandle[2] = &Motor_ChassisMotors;
    Motor_InitMotorGroup(&Motor_ChassisMotors, Motor_TYPE_RM3508, 4, &hcan1, 0x200);
	  Motor_InitMotor(&Motor_ChassisFontRightMotor,Motor_TYPE_RM3508,0x201,0.1,rm3508_encoder_callback);
	  Motor_InitMotor(&Motor_ChassisFontLeftMotor,Motor_TYPE_RM3508,0x202,0.1,rm3508_encoder_callback);	 
	  Motor_InitMotor(&Motor_ChassisBackLeftMotor,Motor_TYPE_RM3508,0x203,0.1,rm3508_encoder_callback);
	  Motor_InitMotor(&Motor_ChassisBackRightMotor,Motor_TYPE_RM3508,0x204,0.1,rm3508_encoder_callback);	
	  Motor_ChassisMotors.motor_handle[0] = &Motor_ChassisFontRightMotor;
    Motor_ChassisMotors.motor_handle[1] = &Motor_ChassisFontLeftMotor;
    Motor_ChassisMotors.motor_handle[2] = &Motor_ChassisBackLeftMotor;
    Motor_ChassisMotors.motor_handle[3] = &Motor_ChassisBackRightMotor;		
}


/**
	* @brief      电机组的初始化
  * @param      pgroup: 指向电机组的指针
  * @param      type: 电机类型，比如3508，具体看结构体Motor_MotorTypeEnum
  * @param      motor_num: 电机组中的电机数量
  * @param      phcan: CAN 句柄的指针
  * @param      stdid: 电机的 ID
  * @retval     NULL
  */
void Motor_InitMotorGroup(Motor_MotorGroupTypeDef* pgroup, Motor_MotorTypeEnum type, uint8_t motor_num, CAN_HandleTypeDef* phcan, uint16_t stdid) {
    if (pgroup == NULL) return;
    pgroup->motor_num = motor_num;
    pgroup->type = type;

    if (phcan == NULL) return;
    pgroup->can_handle = phcan;
    Can_InitTxHeader(&(pgroup->can_header), stdid, Const_Motor_MOTOR_TX_EXTID, Const_Motor_MOTOR_TX_DLC);

    for (int i = 0; i < 4; ++i) {
        pgroup->motor_handle[i] = NULL;
    }
}


/**
  * @brief      电机初始化
	* @param      pmotor: 指向电机结构体的指针
	* @param      type: 电机的类型，如3508
	* @param      callback: 电机所对应的编码器解码函数
  * @retval     NULL
  */
void Motor_InitMotor(Motor_MotorTypeDef* pmotor, Motor_MotorTypeEnum type, uint16_t id, float fil_param, 
                     Motor_EncoderCallbackFuncTypeDef callback) {
    if (pmotor == NULL) return;
    pmotor->last_update_time = 0;
    pmotor->type = type;
    pmotor->id = id;			
    pmotor->init = 0;			
    pmotor->is_online = 0;
    pmotor->output = 0;
    pmotor->callback = callback;
    Filter_LowPassInit(fil_param, &pmotor->fdb_fil_param);
}


/**
	* @brief      设置电机的输出值
	* @param      pmotor: 指向电机组的指针
  * @param      output: 输出电流值的大小
  * @retval     NULL
  */
void Motor_SetMotorOutput(Motor_MotorTypeDef* pmotor, float output) {
    pmotor->output = output;
}


/**
	* @brief      获取电机的输出值
	* @param      pmotor: 指向电机数据结构体的指针
  * @retval     输出电流
  */
uint16_t Motor_GetMotorOutput(Motor_MotorTypeDef* pmotor) {
    // 如果电机指针为空，返回0表示无效输出
		if (pmotor == NULL) return 0;
    
		if (pmotor->type == Motor_TYPE_NOT_CONNECTED) return 0;
		
		// 对于指定类型的电机，返回电流输出值
    int16_t ret = 0;
    if (pmotor->type == Motor_TYPE_RM3508 || 
				pmotor->type == Motor_TYPE_RM2006 || 
				pmotor->type ==	Motor_TYPE_RM6020	) {
        
				// 计算电流输出值，乘以1000以放大
				ret = (int16_t)(pmotor->output * 1000.0f);
        return (uint16_t)ret;
    }
    return 0;
}


/**
	* @brief      发送电机的电流数据
	* @param      pgroup: 指向要发送的电机组
  * @retval     NULL
  */
void Motor_SendMotorGroupOutput(Motor_MotorGroupTypeDef *pgroup) {
    if (pgroup == NULL) return;
    uint8_t txdata[64];
		memset(txdata, 0, 64);
    txdata[0] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[0]) >> 8);
    txdata[1] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[0]);
    txdata[2] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[1]) >> 8);
    txdata[3] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[1]);
    txdata[4] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[2]) >> 8);
    txdata[5] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[2]);
    txdata[6] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[3]) >> 8);
    txdata[7] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[3]);
    Can_SendMessage(pgroup->can_handle, &(pgroup->can_header), txdata);
}


 /**
  * @brief  这个函数是用来开环控制底盘各个电机的
	* @brief	这个函数仅做示范
  * @param  output1: 电机1的电流值
  * @retval None
  */
void Chassis_motor_control_withoutfedback(float output1,float output2,float output3,float output4)
{
	Motor_SetMotorOutput(&Motor_ChassisFontRightMotor, output1);
	Motor_SetMotorOutput(&Motor_ChassisFontLeftMotor, output2);
	Motor_SetMotorOutput(&Motor_ChassisBackLeftMotor, output3);
	Motor_SetMotorOutput(&Motor_ChassisBackRightMotor, output4);	

	Motor_SendMotorGroupOutput(&Motor_ChassisMotors);	
}



/********** 编码器的回调函数，具体在stm32xx.it.C文件中调用 **********/
/**
  * @brief      用于处理电机编码器解码的回调函数
  * @param      phcan: 	指向 CAN 句柄的指针，表示哪个 CAN 接口被使用
  * @param      stdid: 	CAN 标识符，用于区分不同的 CAN 消息
  * @param      rxdata: 接收数据的缓冲区，包含了从 CAN 总线接收到的数据
  * @param      len:  	接收到的数据长度
  * @retval     NULL
  */
void Motor_EncoderDecodeCallback(CAN_HandleTypeDef* phcan, uint32_t stdid, uint8_t rxdata[], uint32_t len) {
    if (phcan == NULL) {
        return; // 如果 phcan 为 NULL，直接返回，避免空指针问题
    }

    for (int i = 0; i < MOTOR_GROUP_NUM; i++) {
        Motor_MotorGroupTypeDef* group = Motor_groupHandle[i]; // 获取当前电机组的句柄

        // 检查当前电机组是否与接收的 CAN 句柄匹配
        if (phcan != group->can_handle) {
            continue; // 如果不匹配，跳过当前组
        }

        for (int j = 0; j < group->motor_num; j++) {
            Motor_MotorTypeDef* motor = group->motor_handle[j]; // 获取当前电机的句柄

            // 检查 CAN 标识符是否与当前电机匹配
            if (stdid == motor->id) {
                // 调用电机的回调函数进行数据处理，注意，这里的回调是个空的函数指针，要在初始化中选择
                motor->callback(motor, rxdata, len);
                return; // 找到匹配的电机后立即返回
            }
        }
    }
}


/********** 编码器解析函数，以下函数都是在初始化中赋值到结构体就可以了 **********/

/**
	* @brief      rm2006 电机编码器回调
  * @param      pmotor: 指向Motor_MotorTypeDef句柄的指针
  * @retval     NULL
  */
void rm2006_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    // 检查电机句柄是否为空，或接收到的数据长度是否为8字节，如果不满足则直接返回
		if (pmotor == NULL) return;
    if (len != 8) return;
	
    // 更新电机的上一次角度，如果电机已经初始化，则使用当前角度作为上一次角度
    // 否则，从接收到的数据中解析得到角度并存储为上一次角度	
    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    
		// 从接收到的数据中解析当前角度、速度和电流
		pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3])) / 36.0f;
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp = 0; // 电机温度初始化为0
    pmotor->init = 1; // 标记电机已初始化

		// 计算角度变化量
    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      
    
		// 根据角度变化量调整转动圈数计数
		if (diff < -4096)           
        pmotor->encoder.round_count++;
    else if (diff > 4096)       
        pmotor->encoder.round_count--;
		
    // 计算实际的轴角度，需根据减速比除以36
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 10.0f + 
                                       (float)pmotor->encoder.angle / 8192.0f * 10.0f;
    // 防止圈数计数超出范围，进行复位处理
		if (pmotor->encoder.round_count > 10000) {
        pmotor->encoder.consequent_angle -= 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.round_count < -10000) {
        pmotor->encoder.consequent_angle += 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
		
		// 限制角度的范围以防止超出界限，并根据初始偏移量进行调整
    if (pmotor->encoder.limited_angle < pmotor->encoder.init_offset - 180 && pmotor->encoder.init_offset >= 180)
        pmotor->encoder.limited_angle += 360;
    else if (pmotor->encoder.limited_angle > pmotor->encoder.init_offset + 180 && pmotor->encoder.init_offset < 180)
        pmotor->encoder.limited_angle -= 360;
    pmotor->last_update_time = HAL_GetTick(); 
}


/**
	* @brief      rm6020 电机编码器回调
  * @param      pmotor: 指向Motor_MotorTypeDef句柄的指针
  * @retval     NULL
  */
void gm6020_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    // 计算角度差和圈数
    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      //累积转动的角度，正值表示角度增加
    if (diff < -5500)           // 当差值过小时，说明发生正向旋转，圈数加一
        pmotor->encoder.round_count++;
    else if (diff > 5500)       // 当差值过大时，说明发生反向旋转，圈数减一
        pmotor->encoder.round_count--;

	  // 更新电机的上一次角度，如果电机已经初始化，则使用当前角度作为上一次角度
    // 否则，从接收到的数据中解析得到角度并存储为上一次角度	
    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    
		// 从接收到的数据中解析当前角度、速度、电流和温度
		pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3]));
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp = 0; 
    pmotor->init = 1; 

    // 计算转动的里程，圈数加当前角度
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 360.0f + 
                                       (float)pmotor->encoder.angle / 8192.0f * 360.0f;

		// 防止圈数计数超出范围，进行复位处理
		if (pmotor->encoder.round_count > 10000) {
        pmotor->encoder.consequent_angle -= 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.round_count < -10000) {
        pmotor->encoder.consequent_angle += 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    
		// 计算当前角度
		pmotor->encoder.limited_angle = (float)pmotor->encoder.angle / 8192.0f * 360.0f;
}


/**
  * @brief      rm3508 电机编码器回调
  * @param      pmotor: 指向Motor_MotorTypeDef句柄的指针
  * @retval     NULL
  */
void rm3508_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    if (pmotor == NULL) return;
    

    // 更新角度
    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    
		// 从接收到的数据中解析当前角度、速度、电流和温度
		pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3])) / 19.0f;
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp    = (float)((int16_t)((uint16_t)rxbuff[6]));
    
		// 计算角度差异并更新圈数计数
    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      
    if (diff < -4096)           
        pmotor->encoder.round_count++;
    else if (diff > 4096)       
        pmotor->encoder.round_count--;
    
		 // 计算实际的轴角度，需要除以减速比19
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 360.0f / 19.0f + 
                                       (float)pmotor->encoder.angle / 8192.0f * 360.0f / 19.0f;
    
		// 防止圈数计数超出范围，进行复位处理
		if (pmotor->encoder.round_count > 10000) {
        pmotor->encoder.consequent_angle -= 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.round_count < -10000) {
        pmotor->encoder.consequent_angle += 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
		
		// 限制角度的范围以防止超出界限，并根据初始偏移量进行调整
    if (pmotor->encoder.limited_angle < pmotor->encoder.init_offset - 180 && pmotor->encoder.init_offset >= 180)
        pmotor->encoder.limited_angle += 360;
    else if (pmotor->encoder.limited_angle > pmotor->encoder.init_offset + 180 && pmotor->encoder.init_offset < 180)
        pmotor->encoder.limited_angle -= 360;	
		
		// 更新电机状态的最后更新时间戳
    pmotor->last_update_time = HAL_GetTick(); 
}


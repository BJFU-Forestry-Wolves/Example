/*
 *  Project      : 
 * 
 *  FilePath     : 
 *  Description  : 
 *  LastEditors  : Lee
 *  Date         : 2024年8月19日01:52:18
 *  LastEditTime : 
 */


#ifndef DRV_MOTORY
#define DRV_MOTORY

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "alg_filter.h"
	
	
#define 	MOTOR_GROUP_NUM 			3



typedef enum {      
    Motor_TYPE_NOT_CONNECTED    = 0,
    Motor_TYPE_RM3508           = 1,
    Motor_TYPE_RM2006           = 2,
    Motor_TYPE_RM6020           = 3,
	} Motor_MotorTypeEnum;  // 电机的型号，未连接则是0

	
typedef struct {
		// 电机的反馈值
    float angle;
    float speed;
    float current;
    float temp;               // 温度，单位(℃)
    uint8_t error_code;				//

    // 用于计算连续的角度
    float last_angle;
    int32_t round_count;			//圈数
    float init_offset;				//初始偏移量
    float limited_angle;			//限制角度
    float consequent_angle;		//角度值
	} Motor_EncoderTypeDef;			//电机编码器数据
	
	
typedef struct _motor_type {
    Motor_EncoderTypeDef encoder;
    Motor_MotorTypeEnum type;				// 枚举类型，用于标识电机的种类

    float output;
    uint32_t id;
    uint8_t is_online;							// 电机是否在线的状态标识，1 表示在线，0 表示离线
    uint32_t last_update_time;
    uint8_t init;										// 标识电机是否已初始化，1 表示已初始化，0 表示未初始化
    void (*callback)(struct _motor_type*, uint8_t rxbuff[], uint32_t len);
    Filter_LowPassParamTypeDef fdb_fil_param;
    Filter_LowPassTypeDef fdb_fil;
	} Motor_MotorTypeDef;							//控制电机的相关数据
	
typedef struct {
    uint8_t motor_num;
    Motor_MotorTypeDef* motor_handle[4];
    Motor_MotorTypeEnum type;
    CAN_HandleTypeDef* can_handle;
    CAN_TxHeaderTypeDef can_header;
	} Motor_MotorGroupTypeDef;					//电机的分组，包含了ID等CAN通信所需要的数据

	/*函数指针*/
typedef void (*Motor_EncoderCallbackFuncTypeDef)(Motor_MotorTypeDef*, uint8_t[], uint32_t);	

void Motor_InitAllMotors();
	
void Motor_InitMotorGroup(Motor_MotorGroupTypeDef* pgroup, Motor_MotorTypeEnum type, uint8_t motor_num, CAN_HandleTypeDef* phcan, uint16_t stdid);	
void Motor_InitMotor(Motor_MotorTypeDef* pmotor, Motor_MotorTypeEnum type, uint16_t id, float fil_param, Motor_EncoderCallbackFuncTypeDef callback);
	
void Chassis_motor_control_withoutfedback(float output1,float output2,float output3,float output4);	
	
void Motor_EncoderDecodeCallback(CAN_HandleTypeDef* phcan, uint32_t stdid, uint8_t rxdata[], uint32_t len);

void rm2006_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);	
void gm6020_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);
void rm3508_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);	
	
#ifdef __cplusplus
}
#endif

#endif

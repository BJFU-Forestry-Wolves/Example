/*
 *  Project      : 
 * 
 *  FilePath     : module_chassis.h
 *  Description  : Lee
 *  LastEditors  : Lee
 *  Date         : 2024年8月23日22:04:26
 *  LastEditTime : 
 */


#ifndef __MODULE_CHASSIS_H
#define __MODULE_CHASSIS_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "drv_motor.h"
#include "alg_math.h"
#include "alg_pid.h"

typedef enum {
		Chassis_NULL = 0u,
		Chassis_SEP = 1u,
		Chassis_FOLLOW = 2u,
		Chassis_XTL = 3u
}Chassis_ModeEnum;	

typedef struct {
    float chassis_ref;                                  
    float chassis_position_fdb;                         
    float chassis_speed_fdb;                            
    uint8_t chassis_ref_limit_status;                   
    uint8_t chassis_count;

    uint8_t control_state;                          
    uint8_t output_state;                          
    uint8_t pending_state;                          

    PID_PIDTypeDef spdPID;
    PID_PIDParamTypeDef spdPIDParam;
    PID_PIDTypeDef angPID;
    PID_PIDParamTypeDef angPIDParam;

} Chassis_ChassisTypeDef;

typedef struct {
	  Chassis_ModeEnum chassis_mode;
	
		float Chassis_Vx;
		float Chassis_Vy;
		float Chassis_Wz;
	
		float Chassis_Yaw_Angle;													
		float Chassis_Yaw_Rad;
		
		float Chassis_FontRight_Speed;
		float Chassis_FontLeft_Speed;
		float Chassis_BackLeft_Speed;
		float Chassis_BackRight_Speed;
	
} Chassis_StatusTypeDef;


Chassis_ChassisTypeDef* Chassis_ChassisPtr();


void Chassis_InitChassis();
void Chassis_Control();
void Chassis_Output();

void Chassis_motor_control_withPID(float set_output1,float set_output2,float set_output3,float set_output4);
void Chassis_motor_control_withoutPID(float set_output1,float set_output2,float set_output3,float set_output4);

Chassis_ChassisTypeDef* Chassis_ChassisPtr();
Chassis_StatusTypeDef* Chassis_StatusPtr();
#endif

#ifdef __cplusplus
}
#endif

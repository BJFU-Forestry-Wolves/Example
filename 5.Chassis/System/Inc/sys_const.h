/*
 *  Project      : 
 *  
 *  FilePath     : sys_const.h
 *  Description  : 这个文件包含了全部的可调参数
 *  LastEditors  : Lee
 *  Date         : 2024年8月25日17:09:19
 *  LastEditTime : 
 */


#ifndef SYS_CONST_H
#define SYS_CONST_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "main.h"

extern const float REMOTE_CHASSIS_VX_GAIN;            
extern const float REMOTE_CHASSIS_VY_GAIN;								
extern const float REMOTE_CHASSIS_SEP_WZ_GAIN;						
extern const float REMOTE_CHASSIS_FOLLOW_WZ_GAIN;					
extern const float REMOTE_CHASSIS_XTL_WZ_GAIN;					
extern const float CHASSIS_YAW_ANGLE_OFFSET;					
extern const float CHASSIS_XTL_WZ;		


extern const float Const_ChassisFontRightAngParam[4][5];
extern const float Const_ChassisFontRightSpdParam[4][5];
extern const float Const_ChassisFontLeftAngParam[4][5];
extern const float Const_ChassisFontLeftSpdParam[4][5];
extern const float Const_ChassisBackLeftAngParam[4][5];
extern const float Const_ChassisBackLeftSpdParam[4][5];
extern const float Const_ChassisBackRightAngParam[4][5];
extern const float Const_ChassisBackRightSpdParam[4][5];

#endif

#ifdef __cplusplus
}
#endif

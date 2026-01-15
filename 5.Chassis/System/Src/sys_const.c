/*
 *  Project      : 
 *  
 *  FilePath     : sys_const.c
 *  Description  : 这个文件包含了全部的可调参数
 *  LastEditors  : Lee
 *  Date         : 2024年8月25日17:09:19
 *  LastEditTime : 
 */


#include "sys_const.h"


//遥控器增益系数，用于将遥控器值和电机转速值在数量级上统一。你也可以理解为遥控器的灵敏系数。
//可通过更改符号改变方向
const float REMOTE_CHASSIS_VX_GAIN                  = 0.5f;
const float REMOTE_CHASSIS_VY_GAIN									= -0.5f;
const float REMOTE_CHASSIS_SEP_WZ_GAIN							= -0.25f;
const float REMOTE_CHASSIS_FOLLOW_WZ_GAIN						= 5.0f;
const float REMOTE_CHASSIS_XTL_WZ_GAIN							= 1.0f;
const float CHASSIS_YAW_ANGLE_OFFSET								= 210.0f;
const float CHASSIS_XTL_WZ													= 100.0f;



// 底盘PID参数
const float Const_ChassisFontRightAngParam[4][5] = {
    {0.8f, 0, 0, 0, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};
const float Const_ChassisFontRightSpdParam[4][5] = {
    {0.04f, 0.01f, 0.04, 40, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};
const float Const_ChassisFontLeftAngParam[4][5] = {
    {0.8f, 0, 0, 0, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};
const float Const_ChassisFontLeftSpdParam[4][5] = {
    {0.04f, 0.01f, 0.04, 40, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};
const float Const_ChassisBackLeftAngParam[4][5] = {
    {0.8f, 0, 0, 0, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};
const float Const_ChassisBackLeftSpdParam[4][5] = {
    {0.04f, 0.01f, 0.04, 40, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};
const float Const_ChassisBackRightAngParam[4][5] = {
    {0.8f, 0, 0, 0, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};
const float Const_ChassisBackRightSpdParam[4][5] = {
    {0.04f, 0.01f, 0.04, 40, 20.0f}, {0.1f, -1}, {0, 0}, {-1, -1}};






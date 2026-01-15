/*
 *  Project      : 
 * 
 *  file         : app_remote.h
 *  Description  : 本文件用来处理遥控器线程
 *  LastEditors  : Lee
 *  Date         : 2024年8月25日22:14:49
 *  LastEditTime : 
 */


#ifndef APP_REMOTE_H
#define APP_REMOTE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "drv_remote.h"
#include "alg_math.h"

typedef struct {
    uint8_t pending;
    float yaw_angle_offset;
} Remote_RemoteControlTypeDef;

void Remote_Task(void const * argument);
void Remote_ControlCom();
void Remote_ChissisProcess();
void Remote_RemoteProcess();

#endif

#ifdef __cplusplus
}
#endif

/*
 *  Project      : 
 *  
 *  FilePath     : app_init.h
 *  Description  : All initialization threads
 *  LastEditors  : Lee
 *  Date         : 2024年8月17日00:28:24
 *  LastEditTime : 
 */


#ifndef APP_INIT_H
#define APP_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "drv_remote.h"
#include "drv_motor.h"
#include "bsp_can.h"
#include "module_chassis.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"


void Init_InitAll(void);


#endif

#ifdef __cplusplus
}

#endif

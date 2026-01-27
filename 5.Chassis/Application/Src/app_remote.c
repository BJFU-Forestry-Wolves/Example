/*
 *  Project      : 
 * 
 *  file         : app_remote.c
 *  Description  : 本文件用来处理遥控器线程
 *  LastEditors  : Lee
 *  Date         : 2024年8月25日22:14:49
 *  LastEditTime : 
 */

#include "main.h"
#include "app_remote.h"
#include "drv_remote.h"
#include "module_chassis.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"

#define REMOTE_TASK_PERIOD  1

Remote_RemoteControlTypeDef Remote_remoteControlData;
Remote_RemoteDataTypeDef *testdata;

/**
  * @brief          Remote task
  * @param          NULL
  * @retval         NULL
  */
void Remote_Task(void const * argument) {

    for(;;) {
        Remote_ControlCom();
				osDelay(REMOTE_TASK_PERIOD);
    }
}


/**
* @brief      遥控器控制切换
* @param      NULL
* @retval     NULL
*/

void Remote_ControlCom() {
    Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();
		testdata = data;

    switch (data->remote.s[0]) {
		/*	右侧拨杆，选择模式		*/
			
        case Remote_SWITCH_UP: {
            /* 拨杆向上，选择底盘控制模式 */
            Remote_ChissisProcess();
            break;
        }
        case Remote_SWITCH_MIDDLE: {
						break;
        }
        case Remote_SWITCH_DOWN: {
            break;
        }
        default:
            break;
    }
}


/**
* @brief      遥控器控制底盘
* @param      本函数智能控制底盘，用于例程演示，不会在实际中使用到，可删除
* @retval     NULL
*/
void Remote_ChissisProcess() {

    Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();

		
    switch (data->remote.s[1]) {
    /*	选择左侧推杆状态	*/
			
        case Remote_SWITCH_UP: {
						/*	左侧推杆向上，底盘下电	*/
						Chassis_SetChassisMode(Chassis_NULL);
						Chassis_SetChassisRef(NULL,NULL,NULL);
            break;
        }
				
        case Remote_SWITCH_MIDDLE: {
						/*	左侧推杆居中,	控制底盘运动*/
						Chassis_SetChassisMode(Chassis_SEP);
						Chassis_SetChassisRef((float)data->remote.ch[1]  , (float)data->remote.ch[0] , NULL);
            break;
        }
				
        case Remote_SWITCH_DOWN: {
            /* 左侧推杆向下,电机下电   */
						Chassis_SetChassisMode(Chassis_NULL);
						Chassis_SetChassisRef(NULL,NULL,NULL);
            break;
        }
        default:
            break;
    }

}



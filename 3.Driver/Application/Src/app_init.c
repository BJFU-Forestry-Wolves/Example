#include "app_init.h"
#include "drv_motor.h"
#include "bsp_can.h"


void Init_InitAll() {
	
		// 电机初始化
		Can_InitFilterAndStart(&hcan1);
		Can_InitFilterAndStart(&hcan2);
		Motor_InitAllMotors();
	
}

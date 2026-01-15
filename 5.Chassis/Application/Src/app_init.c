#include "app_init.h"


void Init_InitAll() {
		//遥控器初始化
		Remote_InitRemote();
	
		// 电机初始化
		Can_InitFilterAndStart(&hcan1);
		Can_InitFilterAndStart(&hcan2);
		Motor_InitAllMotors();
	
	
		//底盘初始化
		Chassis_InitChassis();
}

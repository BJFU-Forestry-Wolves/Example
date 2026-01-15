/*
 *  Project      : 
 * 
 *  FilePath     : module_chassis.c
 *  Description  : Lee
 *  LastEditors  : Lee
 *  Date         : 2024年8月23日22:04:26
 *  LastEditTime : 
 */
 
 #include "module_chassis.h"
 #include "sys_const.h"
 #include "drv_motor.h"
 
Chassis_ChassisTypeDef Chassis_ControlData[4];
Chassis_StatusTypeDef Chassis_StatusData;


/**
  * @brief      本函数用于初始化底盘
  * @param      NULL
  * @retval     NULL
  */
void Chassis_InitChassis(){
		Chassis_ChassisTypeDef *chassis = Chassis_ChassisPtr();

	  for(int i = 0; i < 4 ; i++)
		{
    chassis[i].control_state = 1;
    chassis[i].output_state = 1;
    chassis[i].chassis_ref = 0;
    chassis[i].chassis_count = 0;		
		}
		PID_InitPIDParam(&chassis[0].spdPIDParam, Const_ChassisFontRightSpdParam[0][0], Const_ChassisFontRightSpdParam[0][1], Const_ChassisFontRightSpdParam[0][2], Const_ChassisFontRightSpdParam[0][3], 
                    Const_ChassisFontRightSpdParam[0][4], Const_ChassisFontRightSpdParam[1][0], Const_ChassisFontRightSpdParam[1][1], Const_ChassisFontRightSpdParam[2][0], Const_ChassisFontRightSpdParam[2][1], 
                    Const_ChassisFontRightSpdParam[3][0], Const_ChassisFontRightSpdParam[3][1], PID_POSITION);
    PID_InitPIDParam(&chassis[0].angPIDParam, Const_ChassisFontRightAngParam[0][0], Const_ChassisFontRightAngParam[0][1], Const_ChassisFontRightAngParam[0][2], Const_ChassisFontRightAngParam[0][3], 
                    Const_ChassisFontRightAngParam[0][4], Const_ChassisFontRightAngParam[1][0], Const_ChassisFontRightAngParam[1][1], Const_ChassisFontRightAngParam[2][0], Const_ChassisFontRightAngParam[2][1], 
                    Const_ChassisFontRightAngParam[3][0], Const_ChassisFontRightAngParam[3][1], PID_POSITION);	

		PID_InitPIDParam(&chassis[1].spdPIDParam, Const_ChassisFontLeftSpdParam[0][0], Const_ChassisFontLeftSpdParam[0][1], Const_ChassisFontLeftSpdParam[0][2], Const_ChassisFontLeftSpdParam[0][3], 
                    Const_ChassisFontLeftSpdParam[0][4], Const_ChassisFontLeftSpdParam[1][0], Const_ChassisFontLeftSpdParam[1][1], Const_ChassisFontLeftSpdParam[2][0], Const_ChassisFontLeftSpdParam[2][1], 
                    Const_ChassisFontLeftSpdParam[3][0], Const_ChassisFontLeftSpdParam[3][1], PID_POSITION);
    PID_InitPIDParam(&chassis[1].angPIDParam, Const_ChassisFontLeftAngParam[0][0], Const_ChassisFontLeftAngParam[0][1], Const_ChassisFontLeftAngParam[0][2], Const_ChassisFontLeftAngParam[0][3], 
                    Const_ChassisFontLeftAngParam[0][4], Const_ChassisFontLeftAngParam[1][0], Const_ChassisFontLeftAngParam[1][1], Const_ChassisFontLeftAngParam[2][0], Const_ChassisFontLeftAngParam[2][1], 
                    Const_ChassisFontLeftAngParam[3][0], Const_ChassisFontLeftAngParam[3][1], PID_POSITION);		

		PID_InitPIDParam(&chassis[2].spdPIDParam, Const_ChassisBackLeftSpdParam[0][0], Const_ChassisBackLeftSpdParam[0][1], Const_ChassisBackLeftSpdParam[0][2], Const_ChassisBackLeftSpdParam[0][3], 
                    Const_ChassisBackLeftSpdParam[0][4], Const_ChassisBackLeftSpdParam[1][0], Const_ChassisBackLeftSpdParam[1][1], Const_ChassisBackLeftSpdParam[2][0], Const_ChassisBackLeftSpdParam[2][1], 
                    Const_ChassisBackLeftSpdParam[3][0], Const_ChassisBackLeftSpdParam[3][1], PID_POSITION);
    PID_InitPIDParam(&chassis[2].angPIDParam, Const_ChassisBackLeftAngParam[0][0], Const_ChassisBackLeftAngParam[0][1], Const_ChassisBackLeftAngParam[0][2], Const_ChassisBackLeftAngParam[0][3], 
                    Const_ChassisBackLeftAngParam[0][4], Const_ChassisBackLeftAngParam[1][0], Const_ChassisBackLeftAngParam[1][1], Const_ChassisBackLeftAngParam[2][0], Const_ChassisBackLeftAngParam[2][1], 
                    Const_ChassisBackLeftAngParam[3][0], Const_ChassisBackLeftAngParam[3][1], PID_POSITION); 
		
		PID_InitPIDParam(&chassis[3].spdPIDParam, Const_ChassisBackRightSpdParam[0][0], Const_ChassisBackRightSpdParam[0][1], Const_ChassisBackRightSpdParam[0][2], Const_ChassisBackRightSpdParam[0][3], 
                    Const_ChassisBackRightSpdParam[0][4], Const_ChassisBackRightSpdParam[1][0], Const_ChassisBackRightSpdParam[1][1], Const_ChassisBackRightSpdParam[2][0], Const_ChassisBackRightSpdParam[2][1], 
                    Const_ChassisBackRightSpdParam[3][0], Const_ChassisBackRightSpdParam[3][1], PID_POSITION);
    PID_InitPIDParam(&chassis[3].angPIDParam, Const_ChassisBackRightAngParam[0][0], Const_ChassisBackRightAngParam[0][1], Const_ChassisBackRightAngParam[0][2], Const_ChassisBackRightAngParam[0][3], 
                    Const_ChassisBackRightAngParam[0][4], Const_ChassisBackRightAngParam[1][0], Const_ChassisBackRightAngParam[1][1], Const_ChassisBackRightAngParam[2][0], Const_ChassisBackRightAngParam[2][1], 
                    Const_ChassisBackRightAngParam[3][0], Const_ChassisBackRightAngParam[3][1], PID_POSITION); 	
		
};


/**
	* @brief      底盘控制函数，使用PID计算并赋值
  * @param      NULL
  * @retval     NULL
  */
void Chassis_Control()
{
		Chassis_ChassisTypeDef *chassis = Chassis_ChassisPtr();
		Chassis_StatusTypeDef *chassis_status = Chassis_StatusPtr();

		for(int i = 0 ; i < 4 ; i++ )
    {
			if(chassis[i].control_state != 1) return;
    }

		//FontRight右前方电机，ID1,对输出值进行PID计算
    PID_SetPIDRef(&chassis[0].spdPID, chassis_status->Chassis_FontRight_Speed);
    PID_SetPIDFdb(&chassis[0].spdPID, Motor_ChassisFontRightMotor.encoder.speed);
    PID_CalcPID(&chassis[0].spdPID, &chassis[0].spdPIDParam);  

		//将PID计算结果存入结构体
		Motor_SetMotorOutput(&Motor_ChassisFontRightMotor, PID_GetPIDOutput(&chassis[0].spdPID));

    //FontLeft
		PID_SetPIDRef(&chassis[1].spdPID, chassis_status->Chassis_FontLeft_Speed);
    PID_SetPIDFdb(&chassis[1].spdPID, Motor_ChassisFontLeftMotor.encoder.speed);
    PID_CalcPID(&chassis[1].spdPID, &chassis[1].spdPIDParam);   

    Motor_SetMotorOutput(&Motor_ChassisFontLeftMotor, PID_GetPIDOutput(&chassis[1].spdPID));

		//BackLeft
    PID_SetPIDRef(&chassis[2].spdPID, chassis_status->Chassis_BackLeft_Speed);
    PID_SetPIDFdb(&chassis[2].spdPID, Motor_ChassisBackLeftMotor.encoder.speed);
    PID_CalcPID(&chassis[2].spdPID, &chassis[2].spdPIDParam);	

    Motor_SetMotorOutput(&Motor_ChassisBackLeftMotor, PID_GetPIDOutput(&chassis[2].spdPID));
		
		//BackRight		
    PID_SetPIDRef(&chassis[3].spdPID, chassis_status->Chassis_BackRight_Speed);
    PID_SetPIDFdb(&chassis[3].spdPID, Motor_ChassisBackRightMotor.encoder.speed);
    PID_CalcPID(&chassis[3].spdPID, &chassis[3].spdPIDParam);   

    Motor_SetMotorOutput(&Motor_ChassisBackRightMotor, PID_GetPIDOutput(&chassis[3].spdPID));		
}


/**
	* @brief      发送电流数据到电机
  * @param      NULL
  * @retval     NULL
  */
void Chassis_Output()
{
		Chassis_ChassisTypeDef *chassis = Chassis_ChassisPtr();

		for(int i = 0 ; i < 4 ; i++ )
    {
			if(chassis[i].output_state != 1) return;
    }
    Motor_SendMotorGroupOutput(&Motor_ChassisMotors);
}

 /**
  * @brief  闭环环控制底盘各个电机
	* @brief	这个函数仅做示范
  * @param  set_output1: 电机1的电流期望值
  * @retval None
  */
void Chassis_motor_control_withPID(float set_output1,float set_output2,float set_output3,float set_output4)
{
		Chassis_ChassisTypeDef *chassis = Chassis_ChassisPtr();
//		Chassis_StatusTypeDef *chassis_status = Chassis_StatusPtr();		
	
		for(int i = 0 ; i < 4 ; i++ )
    {
			if(chassis[i].control_state != 1) return;
    }
		
    PID_SetPIDRef(&chassis[0].spdPID, set_output1);	
    PID_SetPIDFdb(&chassis[0].spdPID, Motor_ChassisFontRightMotor.encoder.speed);		
		PID_CalcPID(&chassis[0].spdPID, &chassis[0].spdPIDParam);
		
		Motor_SetMotorOutput(&Motor_ChassisFontRightMotor, PID_GetPIDOutput(&chassis[0].spdPID));
		
		
    PID_SetPIDRef(&chassis[1].spdPID, set_output2);
    PID_SetPIDFdb(&chassis[1].spdPID, Motor_ChassisFontLeftMotor.encoder.speed);
    PID_CalcPID(&chassis[1].spdPID, &chassis[1].spdPIDParam);   

    Motor_SetMotorOutput(&Motor_ChassisFontLeftMotor, PID_GetPIDOutput(&chassis[1].spdPID));		
		
		
    PID_SetPIDRef(&chassis[2].spdPID, set_output3);
    PID_SetPIDFdb(&chassis[2].spdPID, Motor_ChassisBackLeftMotor.encoder.speed);
    PID_CalcPID(&chassis[2].spdPID, &chassis[2].spdPIDParam);   

    Motor_SetMotorOutput(&Motor_ChassisBackLeftMotor, PID_GetPIDOutput(&chassis[2].spdPID));


    PID_SetPIDRef(&chassis[3].spdPID, set_output4);
    PID_SetPIDFdb(&chassis[3].spdPID, Motor_ChassisBackRightMotor.encoder.speed);
    PID_CalcPID(&chassis[3].spdPID, &chassis[3].spdPIDParam);   

    Motor_SetMotorOutput(&Motor_ChassisBackRightMotor, PID_GetPIDOutput(&chassis[3].spdPID));
}


Chassis_ChassisTypeDef* Chassis_ChassisPtr(){
    return Chassis_ControlData;
}


Chassis_StatusTypeDef* Chassis_StatusPtr(){
		return &Chassis_StatusData;
}


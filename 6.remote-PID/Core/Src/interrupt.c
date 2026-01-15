/*
 *  Project      : 
 * 
 *  FilePath     : 
 *  Description  : 各个中断回调函数都放在这里，方便调用
 *  LastEditors  : Lee
 *  Date         : 2024年8月17日00:44:32
 *  LastEditTime : 
 */

#include "interrupt.h" 
#include "drv_remote.h"




///**
//  * @brief      该函数用于判断是否是空闲中断，放在it.c文件中的uart3的中断回调函数中
//  * @param      huart: uart IRQHandler id
//  * @retval     NULL
//  */
//void Uart_RxIdleCallback(UART_HandleTypeDef* huart) {
//	
//			/*	判断是否是空闲中断，并判断中断源	*/
//		if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE)) {
//				
//			/* 清除空闲中断的标志位 */
//				__HAL_UART_CLEAR_IDLEFLAG(huart);
//			
//			/* 处理遥控器接收到的数据 */		
//				Remote_RXCallback(huart);
//		}

////    if (huart == Const_Referee_UART_HANDLER) {
////        Referee_RXCallback(huart);
////    }
//}


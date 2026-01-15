/*
 *  Project      : 
 * 
 *  FilePath     : periph_remote.c
 *  Description  : 遥控器接收数据相关的文件
 *  LastEditors  : Lee
 *  Date         : 2024年8月17日00:50:59
 *  LastEditTime : 
 */


#include "drv_remote.h"
#include "usart.h"



//接收遥控器数据所用的串口，C板上默认就是UART3，不需要改动
UART_HandleTypeDef* Const_Remote_UART_HANDLER       = &huart3;


/*          遥控器的相关常量，以下数据参考遥控器的手册			    */
#define Const_Remote_RX_BUFF_LEN             				54							
const uint16_t Const_Remote_RX_FRAME_LEN            = 18;
const uint16_t Const_Remote_CHANNEL_VALUE_LIMIT     = 640;
const uint16_t Const_Remote_CHANNEL_VALUE_OFFSET    = 1024;
const uint16_t Const_Remote_CHANNEL_ERROR_LIMIT     = 700;
const uint16_t Const_Remote_REMOTE_OFFLINE_TIME     = 1000;

uint8_t Remote_RxData[Const_Remote_RX_BUFF_LEN];
Remote_RemoteDataTypeDef Remote_RemoteData;



/**
  * @brief      获取遥控器对象指针
  * @param      NULL
  * @retval     
  */
Remote_RemoteDataTypeDef* Remote_GetRemoteDataPtr() {
    return &Remote_RemoteData;
}



/**
  * @brief      遥控器的接收回调函数
  * @param      huart: 遥控器所用串口的指针
	*							huart->hdmarx->Instance->NDTR从 DMA_Stream 的 NDTR寄存器中读取剩余的数据单元数。
  * @retval     NULL
  */
void Remote_RXCallback(UART_HandleTypeDef* huart) {
    /* 关闭DMA */
    __HAL_DMA_DISABLE(huart->hdmarx);

    /* 处理DMA中传输回来的数据 */
    int rxdatalen = Const_Remote_RX_BUFF_LEN - Get_DMACurrentDataCounter(huart->hdmarx->Instance);
		Remote_DecodeRemoteData(Remote_RxData, rxdatalen);

    /* 重新打开DMA */
    __HAL_DMA_SET_COUNTER(huart->hdmarx, Const_Remote_RX_BUFF_LEN);
    __HAL_DMA_ENABLE(huart->hdmarx);
}


/**
  * @brief      遥控器的初始化
  * @param      NULL
  * @retval     NULL
  */
void Remote_InitRemote() {
    Remote_ResetRemoteData();
    Remote_RemoteData.last_update_time = HAL_GetTick();
	
		/*	初始化DMA通道	*/
		__HAL_UART_CLEAR_IDLEFLAG(Const_Remote_UART_HANDLER);
		__HAL_UART_ENABLE_IT(Const_Remote_UART_HANDLER, UART_IT_IDLE);

    uint32_t tmp1 = 0;
//		uint8_t *rxdata = Remote_RxData;  // 使用指针
		
    tmp1 = Const_Remote_UART_HANDLER->RxState;
		if(tmp1 == HAL_UART_STATE_READY){
//			if((Remote_RxData == NULL)	|| (Const_Remote_RX_BUFF_LEN == 0)){
//					return;  // 检查指针是否为空或大小是否为 0
//			}
			Const_Remote_UART_HANDLER->pRxBuffPtr = Remote_RxData;
			Const_Remote_UART_HANDLER->RxXferSize = Const_Remote_RX_BUFF_LEN;
			Const_Remote_UART_HANDLER->ErrorCode  = HAL_UART_ERROR_NONE;
			/* 打开DMA接收通道 */
			HAL_DMA_Start(Const_Remote_UART_HANDLER->hdmarx, (uint32_t)&Const_Remote_UART_HANDLER->Instance->DR, (uint32_t)Remote_RxData, Const_Remote_RX_BUFF_LEN);
			/* 
			 * 启动 DMA 控制器，使其将 UART 接收到的数据自动存储到 Remote_RxData 缓冲区
			 * USART_CR3_DMAR 是 UART 控制寄存器 3 (CR3) 中的一个标志位，用来启用 DMA 接收请求。
			 */
			SET_BIT(Const_Remote_UART_HANDLER->Instance->CR3, USART_CR3_DMAR);
		
		}
}


/**
  * @brief      初始化遥控器各个通道的值
  * @param      
  * @retval     NULL
  */
void Remote_ResetRemoteData() {
    Remote_RemoteDataTypeDef *rc = Remote_GetRemoteDataPtr();
    for (int i = 0; i < 5; ++i)
        rc->remote.ch[i] = 0;
    for (int i = 0; i < 2; ++i)
        rc->remote.s[i] = 0;
    rc->mouse.x = 0;
    rc->mouse.y = 0;
    rc->mouse.z = 0;
    rc->mouse.l = 0;
    rc->mouse.r = 0;
//    Remote_DecodeKeyboardData(&(rc->key), 0);			//返回键盘的值，暂时没写
}



/**
  * @brief      遥控解码
  * @param      rc：指向遥控器数据对象的指针
  * @param      buff: 数据缓存的地址
  * @param      rxdatalen: 数据长度
  * @retval     NULL
  */
void Remote_DecodeRemoteData(uint8_t* buff, int rxdatalen) {
    Remote_RemoteDataTypeDef *rc = Remote_GetRemoteDataPtr();

    if (rxdatalen != Const_Remote_RX_FRAME_LEN) {
        return;                                     //Data length error
    }
    
    rc->state           = Remote_STATE_PENDING;
    rc->last_update_time = HAL_GetTick();   
    rc->remote.ch[0]    = Remote_CancelChannelOffset(((uint16_t)buff[0] | (uint16_t)buff[1] << 8) & 0x07FF);
    rc->remote.ch[1]    = Remote_CancelChannelOffset(((uint16_t)buff[1] >> 3 | (uint16_t)buff[2] << 5) & 0x07FF);
    rc->remote.ch[2]    = Remote_CancelChannelOffset(((uint16_t)buff[2] >> 6 | (uint16_t)buff[3] << 2 | (uint16_t)buff[4] << 10) & 0x07FF);
    rc->remote.ch[3]    = Remote_CancelChannelOffset(((uint16_t)buff[4] >> 1 | (uint16_t)buff[5] << 7) & 0x07FF);
    rc->remote.s[0]     = Remote_ToSwitchState((buff[5] >> 6) & 0x03);
    rc->remote.s[1]     = Remote_ToSwitchState((buff[5] >> 4) & 0x03);
    rc->mouse.x         = ((int16_t)buff[6] | (int16_t)buff[7] << 8);
    rc->mouse.y         = ((int16_t)buff[8] | (int16_t)buff[9] << 8);
    rc->mouse.z         = ((int16_t)buff[10] | (int16_t)buff[11] << 8);
    rc->mouse.l         = buff[12];
    rc->mouse.r         = buff[13];
		//遥控器映射键盘的数据，暂时没写，先留着
		//    Remote_DecodeKeyboardData(&(rc->key), ((int16_t)buff[14]) | ((int16_t)buff[15] << 8));
    /*buff[16],buff[17] Is Dial wheel*/
    rc->remote.ch[4]    = Remote_CancelChannelOffset(((uint16_t)buff[16] | (uint16_t)buff[17] << 8) & 0x07FF);

    if (rc->remote.ch[4] == -1024) rc->remote.ch[4] = 0;

    if (Remote_IsRemoteError()) {
        rc->state       = Remote_STATE_ERROR;
        Remote_ResetRemoteData();
        return;
    }
    rc->state           = Remote_STATE_CONNECTED;
}


/**
  * @brief      判断遥控器是否存在错误
  * @param      rc: 这个指针指向一个包含通道值和开关状态的结构体
	* @retval     0表示正常，1表示错误
  */
uint8_t Remote_IsRemoteError() {
    Remote_RemoteDataTypeDef *rc = Remote_GetRemoteDataPtr();
    const uint8_t REMOTE_OK      = 0;
    const uint8_t REMOTE_ERROR   = 1;

    for (int i = 0; i < 5; ++i)
        if (abs(rc->remote.ch[i]) > Const_Remote_CHANNEL_ERROR_LIMIT) {
            return REMOTE_ERROR;
        }
    for (int i = 0; i < 2; ++i)
        if (rc->remote.s[i] == Remote_SWITCH_NULL) {
            return REMOTE_ERROR;
        }
    return REMOTE_OK;
}



/**
  * @brief      移除遥控器通道值的偏移量，以获取实际的通道值
  * @param      ch: 通道的原始数据
  * @retval     计算后的真实数据
  */
int16_t Remote_CancelChannelOffset(uint16_t ch) {
    return (int16_t) ch - Const_Remote_CHANNEL_VALUE_OFFSET;
}


/**
	* @brief      将原始数据转换为开关状态的枚举类型
  * @param      sw: 遥控器开关的原始数据
  * @retval     遥控器开关的不同状态
  */
Remote_SwitchStateEnum Remote_ToSwitchState(uint8_t sw) {
    return (Remote_SwitchStateEnum) sw;
}


/**
  * @brief      获取当前DMAy Streamx传输中剩余的数据单元数
  * @param      dma_stream: 指向 `DMA_Stream_TypeDef` 结构的指针，
  *             其中y可以是1或2，表示选择的DMA控制器，
  *             x可以是0到7，表示选择的DMA Stream。
  * @retval     当前DMAy Streamx传输中剩余的数据单元数。
  * @note       该函数通过读取DMA流结构体的NDTR寄存器值，获取当前DMA传输中尚未传输的数据单元数。
  *             NDTR寄存器（Number of Data to Transfer Register）存储了DMA传输剩余的数据单元数，
  *             该寄存器的值会在每次数据传输后自动递减。
  */
uint16_t Get_DMACurrentDataCounter(DMA_Stream_TypeDef *dma_stream) {
    /* 返回当前DMAy Streamx传输中剩余的数据单元数 */
    return ((uint16_t)(dma_stream->NDTR));
}




/*
 *  Project      : 
 * 
 *  FilePath     : periph_remote.h
 *  Description  : 遥控器接收的头文件
 *  LastEditors  : Lee
 *  Date         : 2024年8月17日
 *  LastEditTime : 
 */


#ifndef DRV_PERIPH_H
#define DRV_PERIPH_H

#ifdef __cplusplus
extern "C" {
#endif

//#include "drv_remote.h"
#include "main.h"
#include "stdlib.h"
//#include "usart.h"


typedef enum {
    Remote_STATE_NULL       = 0,
    Remote_STATE_CONNECTED  = 1,
    Remote_STATE_LOST       = 2,
    Remote_STATE_ERROR      = 3,
    Remote_STATE_PENDING    = 4
} Remote_RemoteStateEnum;

typedef enum {
    Remote_SWITCH_NULL      = 0,
    Remote_SWITCH_UP        = 1,
    Remote_SWITCH_DOWN      = 2,
    Remote_SWITCH_MIDDLE    = 3
} Remote_SwitchStateEnum;

typedef struct {
    uint8_t w, a, s, d, shift, ctrl, q, e, r, f, g, z, x, c, v, b;
} Remote_KeyboardTypeDef;

typedef struct {
    struct {
				int16_t                 	ch[5];  //遥控器的两个摇杆和边上的拨盘
        Remote_SwitchStateEnum  	s[2];   //边上的两个拨杆
    } remote;

    struct {
        int16_t  x;//mouse x
        int16_t  y;//mouse y
        int16_t  z;//mouse z
        uint8_t  l;//mouse Left key
        uint8_t  r;//mouse Right key
    } mouse;

    Remote_KeyboardTypeDef key;//key board
    Remote_RemoteStateEnum state;
    uint32_t last_update_time;
} Remote_RemoteDataTypeDef;

extern const uint16_t Const_Remote_RX_BUFF_LEN;
extern const uint16_t Const_Remote_RX_FRAME_LEN;
extern const uint16_t Const_Remote_CHANNEL_VALUE_LIMIT;
extern const uint16_t Const_Remote_CHANNEL_VALUE_OFFSET;
extern const uint16_t Const_Remote_CHANNEL_ERROR_LIMIT;
extern const uint16_t Const_Remote_REMOTE_OFFLINE_TIME;


extern UART_HandleTypeDef* Const_Remote_UART_HANDLER;

Remote_RemoteDataTypeDef* Remote_GetRemoteDataPtr(void);

void Remote_InitRemote(void);

void Remote_RXCallback(UART_HandleTypeDef* huart);

//uint8_t Remote_IsRemoteOffline(void);
uint8_t Remote_IsRemoteError(void);

//void Remote_DecodeKeyboardData(Remote_KeyboardTypeDef* key, uint16_t v);
void Remote_DecodeRemoteData(uint8_t* buff, int rxdatalen);

void Remote_ResetRemoteData(void);

int16_t Remote_CancelChannelOffset(uint16_t ch);
Remote_SwitchStateEnum Remote_ToSwitchState(uint8_t sw);

uint16_t Get_DMACurrentDataCounter(DMA_Stream_TypeDef *dma_stream);

#endif

#ifdef __cplusplus
}
#endif

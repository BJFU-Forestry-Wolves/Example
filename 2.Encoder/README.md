# Encoder 项目说明 ✅

## 概述
本工程为基于 STM32F4（STM32F407）的一份编码器/电机固件示例，使用 STM32 HAL 与 FreeRTOS。项目通过 CAN 总线接收电机（RM3508、RM6020、RM2006 等）回传的编码器数据，并在驱动层解析后用于控制与上层算法。

---

## 快速开始 🔧
1. 软件：**Keil MDK-ARM (Keil5)**。
2. 打开工程：进入 `MDK-ARM` 文件夹，**双击** `Encoder.uvprojx` 打开 Keil5。
3. 在 Keil5 中定位源码：点击左侧 `Project` 面板，展开 `Application/User`（或 `Application`）文件夹，打开 `main.c` 查看启动流程与初始化。

---

## CAN 通信实现（重点） 🔌

### 概要流程（接收）
1. 中断入口：`CAN1_RX0_IRQHandler` / `CAN2_RX0_IRQHandler`（`Core/Src/stm32f4xx_it.c`）会调用 HAL 的 `HAL_CAN_IRQHandler()`。
2. HAL 回调：HAL 在收到 FIFO0/FIFO1 有消息时，会触发回调 `HAL_CAN_RxFifo0MsgPendingCallback` 或 `HAL_CAN_RxFifo1MsgPendingCallback`（在 `Core/Src/stm32f4xx_it.c` 文件内定义）。
3. 读取数据：回调中调用 `HAL_CAN_GetRxMessage(phcan, CAN_RX_FIFOx, &Can_RxHeader, Can_RxData)` 将头部与数据复制到全局变量 `Can_RxHeader` 和 `Can_RxData`。缓冲长度由 `Const_Can_RX_BUFF_LEN` 定义（`Bsp/Inc/bsp_can.h`）。
4. 分发处理：随后回调调用 `Can_RxMessageCallback(phcan, &Can_RxHeader, Can_RxData)`（定义在 `Bsp/Src/bsp_can.c`），该函数根据 `phcan` 与 `StdId` 将数据分发到具体处理函数（当前为 `Motor_EncoderDecodeCallback`）。
5. 最终处理：电机处理函数 `Motor_EncoderDecodeCallback(phcan, stdid, rxdata, len)` 在 `App/Driver/Src/drv_motor.c` 中实现，会查找匹配的电机并调用对应的回调（如 `rm3508_encoder_callback` 等）来解析编码器数据并更新电机状态。

### 关键函数与位置 🔍
- 初始化 CAN：`MX_CAN1_Init()`、`MX_CAN2_Init()`（`Core/Src/can.c`）
- 启动滤波并使能接收中断：`Can_InitFilterAndStart(CAN_HandleTypeDef* phcan)`（`Bsp/Src/bsp_can.c`）
- 接收中断处理：`CAN1_RX0_IRQHandler` / `CAN2_RX0_IRQHandler`（`Core/Src/stm32f4xx_it.c`）
- HAL 回调：`HAL_CAN_RxFifo0MsgPendingCallback` / `HAL_CAN_RxFifo1MsgPendingCallback`（`Core/Src/stm32f4xx_it.c`）
- 中间分发：`Can_RxMessageCallback(CAN_HandleTypeDef* phcan, CAN_RxHeaderTypeDef* rxheader, uint8_t rxdata[])`（`Bsp/Src/bsp_can.c`）
- 数据处理（电机）：`Motor_EncoderDecodeCallback(CAN_HandleTypeDef* phcan, uint32_t stdid, uint8_t rxdata[], uint32_t len)`（`App/Driver/Src/drv_motor.c`）
- 发送 CAN：`Can_SendMessage(CAN_HandleTypeDef* phcan, CAN_TxHeaderTypeDef* pheader, uint8_t txdata[])`（`Bsp/Src/bsp_can.c`）

### 相关全局变量
- `Can_RxHeader`：类型 `CAN_RxHeaderTypeDef`，存放收到帧的头（StdId、DLC 等）。
- `Can_RxData[]`：接收数据缓存，大小为 `Const_Can_RX_BUFF_LEN`（在 `Bsp/Inc/bsp_can.h` 定义）。

---

## 引脚与中断配置 ⚙️
- CAN1 引脚：`PD0 = CAN1_RX`, `PD1 = CAN1_TX`（见 `Core/Src/can.c: HAL_CAN_MspInit`）。
- CAN2 引脚：`PB5 = CAN2_RX`, `PB6 = CAN2_TX`（见 `Core/Src/can.c`）。
- 中断向量：`CAN1_RX0_IRQn`, `CAN2_RX0_IRQn` 已在 `can.c` 中启用（NVIC）。
- 滤波器：`Can_InitFilterAndStart` 使用 ID-mask 模式并把消息分配到 FIFO0（可按需修改）。

---

## 使用与调试建议 🛠️
- 无法接收消息：
  - 确认 `Can_InitFilterAndStart()` 是否对相应 `hcan` 被调用并返回 OK（开启滤波 & 启动 CAN & 激活通知）。
  - 检查物理连接、终端电阻（120Ω）、波特率与 `Prescaler/TimeSeg` 配置（`can.c` 中的 `Prescaler`、`TimeSeg1/2`）。
  - 使用示波器或 CAN 分析器检查线上的帧是否存在。
- 要添加新的消息处理：
  - 在 `Can_RxMessageCallback` 中增加对 `StdId` 的判断并调用新的处理函数，或在 `Motor_EncoderDecodeCallback` 中注册/更新电机 ID 与回调。
- 要发送消息：使用 `Can_InitTxHeader()` 先配置 `CAN_TxHeaderTypeDef`，然后 `Can_SendMessage()` 发送。

---

## 关键文件索引 📁
- `Core/Src/main.c` —— 程序入口与外设初始化。 
- `Core/Src/can.c` —— CAN 外设初始化与 GPIO/NVIC 配置。 
- `Core/Src/stm32f4xx_it.c` —— CAN 中断与 HAL 回调（FIFO0/FIFO1 消息回调）。
- `Bsp/Inc/bsp_can.h`, `Bsp/Src/bsp_can.c` —— CAN 辅助函数、滤波、发送与接收分发。
- `App/Driver/Src/drv_motor.c` —— 电机初始化、`Motor_EncoderDecodeCallback` 及各型号编码器解析函数。

---

## 常见问题与解决办法 ❗
- 接收回调不触发：确认 HAL CAN 已 `HAL_CAN_Start()` 并调用 `HAL_CAN_ActivateNotification(... CAN_IT_RX_FIFO0_MSG_PENDING ...)`。
- 中断优先级问题：如与 RTOS 有冲突，检查 NVIC 优先级（`HAL_NVIC_SetPriority`），确保符合 FreeRTOS 中断优先级要求。

---


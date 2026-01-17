# Remote 项目说明 ✅

## 概述
本工程为基于 STM32F4（STM32F407）的一份遥控器固件示例，使用 STM32 HAL 与 FreeRTOS。主功能是通过 USART（USART3）+ DMA + 空闲中断接收遥控器/编码器数据，并在驱动层解析后提供给应用任务处理。

---

## 快速开始 🔧
1. 使用的软件：**Keil MDK-ARM (Keil5)**。
2. 打开工程：进入 `MDK-ARM` 文件夹，**双击** `remote.uvprojx` 打开 Keil5。
3. 在 Keil5 中定位源码：点击左侧 `Project` 面板，展开 `Application/User`（或 `Application`）文件夹，找到并打开 `main.c` 进行查看或调试。

---

## 编译与调试建议 ⚙️
- 推荐使用 **ARM Compiler 6 (CV6)** 编译（源码注释中提及 CV6 与 CV5 的差别）。
- 使用 ST-Link 或其他支持的调试器下载并调试固件。

---

## 硬件引脚与 CubeMX 配置 🧩
- 工程的引脚、外设与中断配置由 **CubeMX** 生成，相关的 CubeMX 工程文件在工程根目录，双击 `.ioc` 文件（例如 `Remote.ioc`）可在 CubeMX 中打开并修改配置。修改后在 CubeMX 中保存并重新生成代码。
- 注意：修改 CubeMX 后请检查生成代码是否覆盖了手写代码区，遵循 `/* USER CODE BEGIN */` / `/* USER CODE END */` 保护手写代码。

---

## 关键文件索引 🔎
- `Core/Src/main.c` —— 程序入口，初始化时钟、外设并启动 FreeRTOS。
- `Core/Src/usart.c`, `Core/Src/dma.c` —— USART / DMA 初始化代码。
- `Core/Src/stm32f4xx_it.c` —— 中断处理（UART 空闲中断处理在此处）。
- `App/Driver/Inc/drv_remote.h`, `App/Driver/Src/drv_remote.c` —— 遥控/编码器数据接收与解析驱动。
- `App/Application/Src/app_init.c` —— 应用层初始化（`Init_InitAll()`）。
- `MDK-ARM/remote.uvprojx` —— Keil 工程文件。
- `.mxproject`, `Remove.ioc` —— CubeMX / 项目配置文件。

---

## 使用与调试小技巧 🛠️
- 若无收到数据：确认硬件 RX 引脚正确连接到 USART3，或在 `Core/Src/gpio.c` 中查看引脚复用设置。
- 可在驱动层 (`drv_remote.c`) 添加日志或调试断点以查看接收到的原始帧。
- UART 接收基于 DMA + 空闲中断，空闲中断处理逻辑请参见 `stm32f4xx_it.c`。

---

## 常见问题与解决办法 ❗
- 编译报错：请优先确认工具链为 ARM Compiler 6，如仍有问题，检查 Keil 的 include 路径与链接设置。
- CubeMX 修改后无效：确认已重新生成代码并将生成文件包含到工程中；审查 `USER CODE` 区是否被误改覆盖。

---

## 其他说明
- 接收实现概述：
  - 中断入口：`USART3_IRQHandler`（位于 `Core/Src/stm32f4xx_it.c`）。该中断函数会调用 `Uart_RxIdleCallback(&huart3)` 来判断是否发生 UART 空闲中断（`UART_FLAG_IDLE`）。
  - 空闲中断处理：在 `Uart_RxIdleCallback` 中检测到空闲中断后，会调用 `Remote_RXCallback(huart)` 来处理接收到的数据。
  - 数据处理流程：`Remote_RXCallback` 先关闭 DMA，计算实际接收到的字节数（通过 `Get_DMACurrentDataCounter` 读取 DMA 的 `NDTR` 寄存器），然后调用 `Remote_DecodeRemoteData(Remote_RxData, rxdatalen)` 对数据进行解析，解析完成后重置 DMA 计数并重新启用 DMA 接收。
  - 相关符号：接收缓存为 `Remote_RxData`，缓冲区长度为 `Const_Remote_RX_BUFF_LEN`，解析函数为 `Remote_DecodeRemoteData`（位于 `App/Driver/Src/drv_remote.c`），初始化由 `Remote_InitRemote` 完成（会开启 DMA 并使能 `UART_IT_IDLE`）。
- 解析实现请参见 `App/Driver/Src/drv_remote.c` 中的 `Remote_DecodeRemoteData`，此处不再重复说明。

---

版权所有 © lee-北京林业大学森林狼战队lee

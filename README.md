# GS_Thread

`GS_Thread` 是一个 **STM32F4 上的 RT-Thread 移植与内核精读工程**：以野火 STM32F407 开发板为载体，把 RT-Thread 官方内核源码原样引入，配合自己编写的板级支持包（BSP）和线程/IPC 测试用例，用于系统性精读线程调度、对象管理、IPC、软件定时器和 Cortex-M 移植机制。

项目目标不是简单堆功能，而是把 RTOS 内核的关键路径拆成可阅读、可验证、可复盘的工程实现。

## 目录导览

| 目录 | 说明 |
| --- | --- |
| `gsthread/` | **RT-Thread 官方内核源码（上游第三方代码，未做任何改动）**。`src/` 为内核主体（thread、scheduler、ipc、object、timer、clock 等），`components/` 为 shell/命令行与环形缓冲区，`include/` 为头文件，`libcpu/` 为 Cortex-M 移植层（含 PendSV 上下文切换汇编） |
| `User/` | **用户自己编写的代码**，本次整理的重心：`main.c` 为应用入口（线程创建与调度器启动），`key/`、`led/`、`usart/` 为按键/LED/调试串口 BSP，`gsconfig.h` 为内核配置，`stm32f4xx_it.c/h` 与 `stm32f4xx_conf.h` 为 ST 库模板的中断与配置文件 |
| `Libraries/` | ST 官方标准外设库（`STM32F4xx_StdPeriph_Driver`）与 CMSIS，均为上游代码 |
| `Project/` | Keil MDK 工程目录（`RVMDK（uv5）/BH-F407.uvprojx`） |
| `Listing/` | Keil 编译产生的链接/汇编列表文件（`.map`、`.lst`） |
| `Output/` | Keil 编译输出（`.hex`/`.axf`/`.o` 等中间产物） |
| `必读说明.txt` | 野火原工程的硬件说明（引脚分配、时钟配置、串口参数） |

## 硬件上下文

- 实验平台：野火 STM32F407 开发板，主控 **STM32F407ZGT6**（168MHz，25MHz 外部晶振）
- 调试串口：USART1（TX=PA9，RX=PA10，115200 8-N-1，中断接收，接到板载 "USB TO UART"）
- LED：R=PF6，G=PF7，B=PF8（RGB 共阳，低电平点亮）
- 按键：KEY1=PA0，KEY2=PC13（按下为高电平）

## 分支说明

| 分支 | 用途 |
| --- | --- |
| `main` | 主分支：当前开发与文档基线，README 与最新代码均以此为准 |
| `master` | 历史分支：main 出现之前的旧主线，具体差异待补充 |
| `release` | 阶段性稳定版本：用于沉淀可回退的学习里程碑，具体标签约定待补充 |

## 构建方式

1. 用 **Keil MDK（µVision 5）** 打开 `Project/RVMDK（uv5）/BH-F407.uvprojx`。
2. 目标芯片已配置为 STM32F407ZGT6，直接编译即可。
3. 通过调试器下载，或取 `Output/Template.hex` 烧录。
4. 串口调试助手连接板载 "USB TO UART"（115200-N-8-1），复位后可观察各线程驱动的 LED 闪烁与 shell 回显。

## 学习路径建议

1. **启动流程**：先读 `User/main.c`——看内核初始化（`gs_thread_all_init`）、IPC 对象创建、6 个测试线程的 `gs_thread_init`/`gs_thread_startup`，以及 `gs_system_scheduler_start` 如何交出 CPU；同时注意本文件提供的 `SysTick_Handler`（系统节拍来源）与 `gsthread/libcpu` 提供的 PendSV 之间的关系。
2. **调度器**：读 `gsthread/src/scheduler.c`（就绪表、优先级位图、上下文切换入口），配合 `gsthread/src/thread.c` 理解线程控制块与状态迁移。
3. **IPC**：读 `gsthread/src/ipc.c`（信号量、互斥锁、挂起/唤醒链表），对照 `main.c` 中 flag4/flag5/flag6 线程里注释掉的实验代码。
4. **内存与基础设施**：读 `gsthread/src/object.c`（对象容器）、`timer.c`/`clock.c`（软件定时器与节拍）、`kservice.c`（内核服务函数）、`list.c`（内核链表）。
5. **设备框架与移植层**：读 `gsthread/components/`（shell、ringbuffer）与 `gsthread/libcpu/arm/`（Cortex-M 上下文切换汇编），理解 RT-Thread 从内核到芯片的最后一公里。

## 维护边界

- `gsthread/` 与 `Libraries/` 为上游代码，**只读不改**；所有实验与修改集中在 `User/`。
- 该项目以理解 RTOS 内核机制为主，不追求替代成熟 RTOS；复杂外设驱动与业务应用应保持克制，避免干扰内核学习主线。

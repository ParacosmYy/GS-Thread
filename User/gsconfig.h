/**
  ******************************************************************************
  * @file    gsconfig.h
  * @brief   gsthread 内核配置头文件：定义线程数量/优先级、内存对齐、
  *          系统节拍频率、IPC（信号量/互斥锁）容量以及 shell 参数。
  *          修改本文件即可裁剪内核功能与资源占用。
  ******************************************************************************
  */

#ifndef __GSTHREAD_CFG_H__
#define __GSTHREAD_CFG_H__


#define GS_THREAD_PRIORITY_MAX      32      /* 最大优先级（就绪表位数） */
#define GS_THREAD_NUM_MAX           100     /* 线程控制块最大个数 */
#define GS_ALIGN_SIZE               4       /* 多少个字节对齐 */

#define GS_NAME_MAX                 15      /* 内核对象的名字长度，字符串形式 */

#define GS_TICK_PER_SECOND          1000    /* 系统节拍频率(Hz)，SysTick 按此频率中断 */

#define GS_USING_SEMAPHORE          1
#define GS_SEM_NUM_MAX              50

#define GS_IPC_FLAG_FIFO            0
#define GS_IPC_FLAG_PRIO            1

#define GS_USING_MUTEX              1
#define GS_MUTEX_NUM_MAX            50

#define GS_CMD_NUM_MAX              200

#define GS_SHELL_STACK_SIZE         1000
#define GS_SHELL_PRIORITY           10

#define GS_SHELL_LENGSH             50      /* 命令行最大长度 */
#define GS_SHELL_HISORY_MAX         30      /* 历史命令最大条数 */

#endif /* __RTTHREAD_CFG_H__ */

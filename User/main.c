/**
  ******************************************************************************
  * @file    main.c
  * @brief   应用程序入口：初始化 gsthread 内核，创建信号量/互斥锁与 6 个测试线程，
  *          随后启动系统调度器。同时在本文件中实现 SysTick 中断服务函数，
  *          为内核提供系统节拍。
  * @attention
  *   硬件平台：野火 STM32F407 开发板（STM32F407ZGT6，168MHz）
  *   LED：PF6/PF7/PF8（低电平点亮，见 led/bsp_led.h）
  *   调试串口：USART1（PA9/PA10，115200 8-N-1，见 usart/bsp_debug_usart.h）
  *   注意：PendSV 与 SysTick 的硬件优先级由内核移植层配置，
  *         SysTick 频率由 GS_TICK_PER_SECOND（gsconfig.h）决定，当前为 1kHz。
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include <gsthread.h>

gs_uint8_t flag1;
gs_uint8_t flag2;
gs_uint8_t flag3;

struct gs_thread gs_thread_flag1;
struct gs_thread gs_thread_flag2;
struct gs_thread gs_thread_flag3;
struct gs_thread gs_thread_flag4;
struct gs_thread gs_thread_flag5;
struct gs_thread gs_thread_flag6;

extern gs_list_t gs_thread_priority_table[GS_THREAD_PRIORITY_MAX];

ALIGN(GS_ALIGN_SIZE)

gs_uint8_t gs_flag1_thread_stack[128];
gs_uint8_t gs_flag2_thread_stack[128];
gs_uint8_t gs_flag3_thread_stack[128];
gs_uint8_t gs_flag4_thread_stack[128];
gs_uint8_t gs_flag5_thread_stack[128];
gs_uint8_t gs_flag6_thread_stack[128];

void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);
void flag3_thread_entry(void *p_arg);
void flag4_thread_entry(void *p_arg);
void flag5_thread_entry(void *p_arg);
void flag6_thread_entry(void *p_arg);

static gs_sem_t sem = GS_NULL;
static gs_mutex_t mutex = GS_NULL;

gs_err_t sem_take, mutex_take;
gs_uint8_t shared_count = 0;


/**
  * @brief  主函数：初始化内核对象与线程，最后启动调度器（不再返回）
  * @param  无
  * @retval 无（调度器启动后不会回到这里）
  */
int main(void)
{
    gs_thread_all_init();

    sem = gs_sem_create("GS_SEM", 1, GS_IPC_FLAG_FIFO);

    mutex = gs_mutex_create("GS_mutex", GS_IPC_FLAG_FIFO);

    //test = gs_ipc_list_resume(gs_thread_priority_table[3].next);

    gs_thread_init(&gs_thread_flag1,
                   "led_red",
                   flag1_thread_entry,
                   GS_NULL,
                   &gs_flag1_thread_stack[0],
                   sizeof(gs_flag1_thread_stack),
                   2,
                   4);

    //gs_list_insert_before(&(gs_thread_priority_table[0]), &(gs_thread_flag1.tlist));
    gs_thread_startup(&gs_thread_flag1);

    gs_thread_init(&gs_thread_flag2,
                   "led_green",
                   flag2_thread_entry,
                   GS_NULL,
                   &gs_flag2_thread_stack[0],
                   sizeof(gs_flag2_thread_stack),
                   4,
                   2);

//  gs_list_insert_before(&(gs_thread_priority_table[1]), &(gs_thread_flag2.tlist));
    gs_thread_startup(&gs_thread_flag2);

    gs_thread_init(&gs_thread_flag3,
                   "led_yellow",
                   flag3_thread_entry,
                   GS_NULL,
                   &gs_flag3_thread_stack[0],
                   sizeof(gs_flag3_thread_stack),
                   4,
                   3);
    gs_thread_startup(&gs_thread_flag3);

    gs_thread_init(&gs_thread_flag4,
                   "SEM",
                   flag4_thread_entry,
                   GS_NULL,
                   &gs_flag4_thread_stack[0],
                   sizeof(gs_flag4_thread_stack),
                   5,
                   3);
    gs_thread_startup(&gs_thread_flag4);

    gs_thread_init(&gs_thread_flag5,
                   "MUTEX_DRV",
                   flag5_thread_entry,
                   GS_NULL,
                   &gs_flag5_thread_stack[0],
                   sizeof(gs_flag5_thread_stack),
                   6,
                   3);
    gs_thread_startup(&gs_thread_flag5);

    gs_thread_init(&gs_thread_flag6,
                   "MUTEX_TST",
                   flag6_thread_entry,
                   GS_NULL,
                   &gs_flag6_thread_stack[0],
                   sizeof(gs_flag6_thread_stack),
                   7,
                   3);
    gs_thread_startup(&gs_thread_flag6);

    gs_system_scheduler_start();

}

/* 不精确软件延时，仅供调试观察用 */
void delay(uint32_t count)
{
    for (; count != 0; count--);
}

/* 线程1：红灯闪烁，周期约 1s（依赖 1kHz 系统节拍） */
void flag1_thread_entry(void *p_arg)
{
    for ( ;; )
    {
        LED1_TOGGLE;
        gs_thread_delay(1000);

    }
}

/* 线程2：绿灯闪烁，周期约 1s */
void flag2_thread_entry(void *p_arg)
{
    for ( ;; )
    {
        LED2_TOGGLE;
        gs_thread_delay_ms(1000);
    }
}

/* 线程3：黄灯（红+绿）闪烁，周期约 1s */
void flag3_thread_entry(void *p_arg)
{
    for ( ;; )
    {
        LED3_TOGGLE;
        gs_thread_delay(1000);

    }
}

void flag5_thread_entry(void *p_arg)
{
    int step;

    while (1)
    {
        gs_thread_delay(500);
//        printf("flag5: 等待互斥锁...\n");
//        mutex_take = gs_mutex_take(mutex, -1);
//        if (mutex_take == GS_EOK)
//        {
//            printf("flag5: 进入临界区\n");
//
//            for (step = 0; step < 5; step++)
//            {
//                shared_count++;
//                printf("flag5: shared_count=%u (step %d)\n", shared_count, step + 1);
//                gs_thread_delay(50);
//            }

//            mutex_take = gs_mutex_release(mutex);
//            if (mutex_take != GS_EOK)
//            {
//                printf("flag5: 释放互斥锁失败，错误码=%d\n", mutex_take);
//            }
//            else
//            {
//                printf("flag5: 已释放互斥锁\n");
//            }
//        }
//        else
//        {
//            printf("flag5: 获取互斥锁失败，错误码=%d\n", mutex_take);
//        }

//        gs_thread_delay(200);
    }
}

void flag6_thread_entry(void *p_arg)
{
//    gs_err_t release_ret;
    while (1)
    {
//        mutex_take = gs_mutex_take(mutex, 300);
//        if (mutex_take == GS_EOK)
//        {
//            shared_count++;
//            printf("flag6: 抢到互斥锁，共享计数=%u\n", shared_count);
//            gs_thread_delay(20);

//            release_ret = gs_mutex_release(mutex);
//            if (release_ret != GS_EOK)
//            {
//                printf("flag6: 释放互斥锁失败，错误码=%d\n", release_ret);
//            }
//            else
//            {
//                printf("flag6: 已释放互斥锁\n");
//            }
//        }
//        else if (mutex_take == -GS_ETIMEOUT)
//        {
//            printf("flag6: 等待互斥锁超时\n");
//        }
//        else
//        {
//            printf("flag6: 获取互斥锁出现错误，错误码=%d\n", mutex_take);
//        }

//        gs_thread_delay(100);
        gs_thread_delay(500);
    }
}

void flag4_thread_entry(void *p_arg)
{
    while (1)
    {
//        sem_take = gs_sem_take(sem, 20);
//        if (sem_take == GS_EOK)
//        {
//            printf("sem获取成功\n");
//        }
//        else if (sem_take == -GS_ETIMEOUT)
//        {
//            printf("sem获取失败，已经超时\n");
//            sem = gs_sem_create("GS_SEM", 1, GS_IPC_FLAG_FIFO);
//        }
//        gs_thread_delay(500);
        gs_thread_delay(500);
    }
}

/**
  * @brief  SysTick 中断服务函数：为 gsthread 内核提供系统节拍
  * @attention 进入/离开中断时必须调用 gs_interrupt_enter/leave，
  *            以便内核进行中断嵌套计数与抢占式调度判断。
  * @param  无
  * @retval 无
  */
void SysTick_Handler(void)
{
    /* 进入中断 */
    gs_interrupt_enter();

    gs_tick_increase();

    /* 离开中断 */
    gs_interrupt_leave();
}




//int main(void)
//{
//  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
//  Debug_USART_Config();
//
//  /* 发送一个字符串 */
//  Usart_SendString(DEBUG_USART, "这是一个串口中断接收回显实验\n");
//  printf("这是一个串口中断接收回显实验\n");
//
//  while (1)
//  {
//
//  }
//}

/*********************************************END OF FILE**********************/

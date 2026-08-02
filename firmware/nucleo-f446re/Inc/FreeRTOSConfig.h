#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

/* SystemCoreClock is updated by SystemInit()/HAL_RCC clock config (CMSIS). */
extern uint32_t SystemCoreClock;

#define configUSE_PREEMPTION                   1
#define configUSE_IDLE_HOOK                    0
#define configUSE_TICK_HOOK                    0
#define configCPU_CLOCK_HZ                     (SystemCoreClock)
#define configTICK_RATE_HZ                     ((TickType_t)1000)
#define configMAX_PRIORITIES                   7
#define configMINIMAL_STACK_SIZE                ((uint16_t)128)
#define configTOTAL_HEAP_SIZE                  ((size_t)(32 * 1024))
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE

#define configUSE_TRACE_FACILITY                0
#define configGENERATE_RUN_TIME_STATS           0
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configUSE_MALLOC_FAILED_HOOK             1

/* API inclusion */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState           1
#define INCLUDE_xTaskGetCurrentTaskHandle         1
#define INCLUDE_uxTaskGetStackHighWaterMark       1

/* Cortex-M4F interrupt priority config (NVIC priority bits: F446 uses 4) */
#define configPRIO_BITS                          4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY    0x0F
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Route the Cortex-M4F port's naked SVC/PendSV handlers directly onto the
 * vector table entries (pure token substitution, so `naked` is preserved).
 * SysTick is NOT aliased here: HAL owns SysTick_Handler (see stm32f4xx_it.c)
 * so HAL_GetTick()/HAL_Delay() keep working before vTaskStartScheduler(). */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

#define configASSERT(x) if ((x) == 0) { taskDISABLE_INTERRUPTS(); for (;;); }

#endif /* FREERTOS_CONFIG_H */

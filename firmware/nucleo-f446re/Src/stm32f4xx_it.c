#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

/* Declared in the ARM_CM4F port but not exposed via a public FreeRTOS header. */
extern void xPortSysTickHandler(void);

void SysTick_Handler(void) {
    HAL_IncTick();

#if (INCLUDE_xTaskGetSchedulerState == 1)
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
#endif
}

#include "FreeRTOS.h"
#include "task.h"

/* configCHECK_FOR_STACK_OVERFLOW == 2 requires this to be defined. */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}

/* configUSE_MALLOC_FAILED_HOOK == 1 requires this to be defined. */
void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}

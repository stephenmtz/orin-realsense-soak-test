# Set the minimum version of CMake that can be used
cmake_minimum_required(VERSION 3.20)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Set target definition variables required by the toolchain file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/target.cmake)

# Set the toolchain file for cross-compilation
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)

# Project definition
project(nucleo-f446re)

# Enable CMake support for ASM and C/CPP languages
enable_language(C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME nucleo-f446re)

# Create an executable object type
add_executable(${PROJECT_NAME})

# Include additional CMake files for flags, components, and files
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/files.cmake)

# cmake/flags.cmake is a CubeMX-managed/regenerated file and hardcodes
# -std=gnu11 even for CXX sources. Appended here (after the include above)
# so this -std wins on the compiler command line without editing that file.
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>
)

# Add sources to executable/library
target_sources(${PROJECT_NAME} PRIVATE
  # User defined sources
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/Src/stm32f4xx_it.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/Src/freertos_hooks.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/tasks.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/list.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/queue.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/timers.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/event_groups.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/stream_buffer.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/portable/MemMang/heap_4.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../drivers/hts221.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../driver_tasks/hts221_task.cpp"
)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  # User defined include directories
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/STM32F4xx_HAL_Driver/Inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CMSIS/Core/Include"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CMSIS/Device/ST/STM32F4xx/Include"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/FreeRTOS/Source/portable/GCC/ARM_CM4F"
  "${CMAKE_CURRENT_SOURCE_DIR}/../drivers"
  "${CMAKE_CURRENT_SOURCE_DIR}/../driver_tasks"
)

# Compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
  # User defined compiler options
)

# Add additional defined symbols
target_compile_definitions(${PROJECT_NAME} PRIVATE
  # Add additional defined symbols here
  STM32F446xx
  USE_HAL_DRIVER
)

# Linker options
target_link_options(${PROJECT_NAME} PRIVATE 
  # User defined linker options
)

# Add additional linker search directories
target_link_directories(${PROJECT_NAME} PRIVATE
  # Add additional linker search directories here
)

# Add additional linker libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
  # Add additional linker libraries here
)

# Flash target: `cmake --build build --target flash`
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/flash.cmake)

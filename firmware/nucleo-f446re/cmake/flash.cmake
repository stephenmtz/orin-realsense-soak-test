find_program(OPENOCD openocd)

if(OPENOCD)
  add_custom_target(flash
    COMMAND ${OPENOCD}
            -f interface/stlink.cfg
            -f target/stm32f4x.cfg
            -c "program $<TARGET_FILE:${PROJECT_NAME}> verify reset exit"
    DEPENDS ${PROJECT_NAME}
    COMMENT "Flashing ${PROJECT_NAME} to target via ST-Link (OpenOCD)"
    USES_TERMINAL
    VERBATIM
  )
else()
  add_custom_target(flash
    COMMAND ${CMAKE_COMMAND} -E echo "openocd not found on PATH - install it to use the flash target"
    COMMAND ${CMAKE_COMMAND} -E false
  )
endif()

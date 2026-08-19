
PRJ_TARGET = chimera3_$(MODE)
PRJ_DEVICE = STM32F407

PRJ_SOURCES =  \
app/src/app_main.c \
app/src/app_soc_cmd_control_func.c \
app/src/app_soc_cmd_event_func.c \
app/src/app_soc_cmd_info_func.c \
app/src/app_soc_cmd_transceiver_func.c \
app/src/app_soc_cmd_update_func.c \
app/src/app_soc_cmd_watchdog_func.c \
app/src/app_soc_cmd.c \
app/src/app_system.c \
hal/chimera3/Firmware/BSP/STM32F4-Discovery/stm32f4_discovery.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sram.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fsmc.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c \
hal/chimera3/src/hal_adc.c \
hal/chimera3/src/hal_flash.c \
hal/chimera3/src/hal_fw_update_cmd_interface.c \
hal/chimera3/src/hal_gpio.c \
hal/chimera3/src/hal_key_handler.c \
hal/chimera3/src/hal_lcb_cmd_interface.c \
hal/chimera3/src/hal_pwm.c \
hal/chimera3/src/hal_soc_cmd_interface.c \
hal/chimera3/src/hal_stm32f4xx_it.c \
hal/chimera3/src/hal_syscalls.c \
hal/chimera3/src/hal_sysclk.c \
hal/chimera3/src/hal_system.c \
hal/chimera3/src/hal_systick.c \
hal/chimera3/src/hal_timer_handler.c \
hal/chimera3/src/hal_uart.c \
hal/chimera3/src/hal_usb.c \
mid/src/mid_comm_monitor_led.c \
mid/src/mid_erp_flow.c \
mid/src/mid_fan.c \
mid/src/mid_lcb_cmd.c \
mid/src/mid_log.c \
mid/src/mid_scheduler.c \
mid/src/mid_soc_cmd.c

PRJ_ASM_SOURCES =  \
hal/chimera3/startup/startup_stm32f407vgtx_app.s

PRJ_LDSCRIPT =  \
hal/chimera3/startup/STM32F407VGTX_FLASH_app.ld

PRJ_AS_DEFS =

PRJ_C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F407xx \
-DUSE_STM32F4_DISCO \
-DAPP_RUN \
-DDebugIrqUsart

PRJ_AS_INCLUDES =  \
-Iapp/inc \
-Ihal/chimera3/Firmware/BSP/STM32F4-Discovery \
-Ihal/chimera3/Firmware/CMSIS/Device/ST/STM32F4xx/Include \
-Ihal/chimera3/Firmware/CMSIS/Include \
-Ihal/chimera3/Firmware/STM32F4xx_HAL_Driver/Inc \
-Ihal/chimera3/Firmware/STM32F4xx_HAL_Driver/Inc/Legacy \
-Ihal/chimera3/inc \
-Ihal/chimera3/Utilities/Include \
-Imid/inc

PRJ_C_INCLUDES = \
-Iapp/inc \
-Ihal/chimera3/Firmware/BSP/STM32F4-Discovery \
-Ihal/chimera3/Firmware/CMSIS/Device/ST/STM32F4xx/Include \
-Ihal/chimera3/Firmware/CMSIS/Include \
-Ihal/chimera3/Firmware/STM32F4xx_HAL_Driver/Inc \
-Ihal/chimera3/Firmware/STM32F4xx_HAL_Driver/Inc/Legacy \
-Ihal/chimera3/inc \
-Ihal/chimera3/Utilities/Include \
-Imid/inc

PRJ_CPU = -mcpu=cortex-m4

PRJ_FPU = -mfpu=fpv4-sp-d16

PRJ_FLOAT-ABI = -mfloat-abi=hard

PRJ_MCU = $(PRJ_CPU) -mthumb $(PRJ_FPU) $(PRJ_FLOAT-ABI)

PRJ_ALL_SOURCES = app/src \
hal/chimera3/Firmware/BSP/STM32F4-Discovery \
hal/chimera3/Firmware/STM32F4xx_HAL_Driver/Src \
hal/chimera3/src \
mid/src
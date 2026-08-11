
PRJ_TARGET = vortex_lite2_$(MODE)
PRJ_DEVICE = GD32F527

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
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_adc.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_can.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_ctc.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_dbg.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_dma.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_exti.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_fmc.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_fwdgt.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_gpio.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_i2c.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_misc.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_pmu.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_rcu.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_rtc.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_spi.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_syscfg.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_timer.c \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_usart.c \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/class/cdc/Source/cdc_acm_core.c \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/core/Source/usbd_core.c \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/core/Source/usbd_enum.c \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/core/Source/usbd_transc.c \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/driver/Source/drv_usb_core.c \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/driver/Source/drv_usb_dev.c \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/driver/Source/drv_usbd_int.c \
hal/vortex_lite2/Utilities/Source/fmc_operation.c \
hal/vortex_lite2/src/hal_adc.c \
hal/vortex_lite2/src/hal_can.c \
hal/vortex_lite2/src/hal_flash.c \
hal/vortex_lite2/src/hal_fw_update_cmd_interface.c \
hal/vortex_lite2/src/hal_gd32f5xx_it.c \
hal/vortex_lite2/src/hal_gpio.c \
hal/vortex_lite2/src/hal_i2c.c \
hal/vortex_lite2/src/hal_key_handler.c \
hal/vortex_lite2/src/hal_lcb_cmd_interface.c \
hal/vortex_lite2/src/hal_power.c \
hal/vortex_lite2/src/hal_pwm.c \
hal/vortex_lite2/src/hal_soc_cmd_interface.c \
hal/vortex_lite2/src/hal_syscalls.c \
hal/vortex_lite2/src/hal_sysclk.c \
hal/vortex_lite2/src/hal_system.c \
hal/vortex_lite2/src/hal_systick.c \
hal/vortex_lite2/src/hal_timer_handler.c \
hal/vortex_lite2/src/hal_uart.c \
hal/vortex_lite2/src/hal_usb.c \
mid/src/mid_bump_sensor.c \
mid/src/mid_can_cmd.c \
mid/src/mid_comm_monitor_led.c \
mid/src/mid_csafe_cab.c \
mid/src/mid_erp_flow.c \
mid/src/mid_fan.c \
mid/src/mid_lcb_cmd.c \
mid/src/mid_log.c \
mid/src/mid_scheduler.c \
mid/src/mid_soc_cmd.c \
mid/src/mid_tca8418_keypad.c

PRJ_ASM_SOURCES =  \
hal/vortex_lite2/startup/startup_gd32f527_app.s

PRJ_LDSCRIPT =  \
hal/vortex_lite2/startup/gd32f527_flash_app.ld

PRJL_AS_DEFS =

PRJ_C_DEFS =  \
-DGD32F527 \
-DUSE_USB_HS \
-DUSE_EMBEDDED_PHY

PRJ_AS_INCLUDES =  \
-Iapp/inc \
-Ihal/vortex_lite2/inc \
-Ihal/vortex_lite2/Firmware/CMSIS/GD/GD32F5xx/Include \
-Ihal/vortex_lite2/Firmware/CMSIS/Core/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/class/cdc/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/core/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/driver/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/ustd/common \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/ustd/class/cdc \
-Ihal/vortex_lite2/Utilities/Include \
-Imid/inc

PRJ_C_INCLUDES = \
-Iapp/inc \
-Ihal/vortex_lite2/inc \
-Ihal/vortex_lite2/Firmware/CMSIS/GD/GD32F5xx/Include \
-Ihal/vortex_lite2/Firmware/CMSIS/Core/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/class/cdc/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/core/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/driver/Include \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/ustd/common \
-Ihal/vortex_lite2/Firmware/GD32F5xx_usb_library/ustd/class/cdc \
-Ihal/vortex_lite2/Utilities/Include \
-Imid/inc

PRJ_CPU = -mcpu=cortex-m33

PRJ_FPU = -mfpu=fpv5-sp-d16

PRJL_FLOAT-ABI = -mfloat-abi=hard

PRJ_MCU = $(PRJ_CPU) -mthumb $(PRJ_FPU) $(PRJ_FLOAT-ABI)

PRJ_ALL_SOURCES = app/src \
hal/vortex_lite2/Firmware/GD32F5xx_standard_peripheral/Source \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/class/cdc/Source \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/device/core/Source \
hal/vortex_lite2/Firmware/GD32F5xx_usb_library/driver/Source \
hal/vortex_lite2/src	\
hal/vortex_lite2/Utilities/Source \
mid/src
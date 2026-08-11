
PRJ_TARGET = ulcgc_$(MODE)
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
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_adc.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_can.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_ctc.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_dbg.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_dma.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_exti.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_fmc.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_fwdgt.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_gpio.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_i2c.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_misc.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_pmu.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_rcu.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_rtc.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_spi.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_syscfg.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_timer.c \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source/gd32f5xx_usart.c \
hal/ulcgc/Firmware/GD32F5xx_usb_library/device/class/cdc/Source/cdc_acm_core.c \
hal/ulcgc/Firmware/GD32F5xx_usb_library/device/core/Source/usbd_core.c \
hal/ulcgc/Firmware/GD32F5xx_usb_library/device/core/Source/usbd_enum.c \
hal/ulcgc/Firmware/GD32F5xx_usb_library/device/core/Source/usbd_transc.c \
hal/ulcgc/Firmware/GD32F5xx_usb_library/driver/Source/drv_usb_core.c \
hal/ulcgc/Firmware/GD32F5xx_usb_library/driver/Source/drv_usb_dev.c \
hal/ulcgc/Firmware/GD32F5xx_usb_library/driver/Source/drv_usbd_int.c \
hal/ulcgc/Utilities/Source/fmc_operation.c \
hal/ulcgc/src/hal_adc.c \
hal/ulcgc/src/hal_can.c \
hal/ulcgc/src/hal_flash.c \
hal/ulcgc/src/hal_fw_update_cmd_interface.c \
hal/ulcgc/src/hal_gd32f5xx_it.c \
hal/ulcgc/src/hal_gpio.c \
hal/ulcgc/src/hal_i2c.c \
hal/ulcgc/src/hal_key_handler.c \
hal/ulcgc/src/hal_lcb_cmd_interface.c \
hal/ulcgc/src/hal_power.c \
hal/ulcgc/src/hal_pwm.c \
hal/ulcgc/src/hal_soc_cmd_interface.c \
hal/ulcgc/src/hal_syscalls.c \
hal/ulcgc/src/hal_sysclk.c \
hal/ulcgc/src/hal_system.c \
hal/ulcgc/src/hal_systick.c \
hal/ulcgc/src/hal_timer_handler.c \
hal/ulcgc/src/hal_uart.c \
hal/ulcgc/src/hal_usb.c \
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
hal/ulcgc/startup/startup_gd32f527_app.s

PRJ_LDSCRIPT =  \
hal/ulcgc/startup/gd32f527_flash_app.ld

PRJL_AS_DEFS =

PRJ_C_DEFS =  \
-DGD32F527 \
-DUSE_USB_HS \
-DUSE_EMBEDDED_PHY

PRJ_AS_INCLUDES =  \
-Iapp/inc \
-Ihal/ulcgc/inc \
-Ihal/ulcgc/Firmware/CMSIS/GD/GD32F5xx/Include \
-Ihal/ulcgc/Firmware/CMSIS/Core/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/device/class/cdc/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/device/core/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/driver/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/ustd/common \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/ustd/class/cdc \
-Ihal/ulcgc/Utilities/Include \
-Imid/inc

PRJ_C_INCLUDES = \
-Iapp/inc \
-Ihal/ulcgc/inc \
-Ihal/ulcgc/Firmware/CMSIS/GD/GD32F5xx/Include \
-Ihal/ulcgc/Firmware/CMSIS/Core/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/device/class/cdc/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/device/core/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/driver/Include \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/ustd/common \
-Ihal/ulcgc/Firmware/GD32F5xx_usb_library/ustd/class/cdc \
-Ihal/ulcgc/Utilities/Include \
-Imid/inc

PRJ_CPU = -mcpu=cortex-m33

PRJ_FPU = -mfpu=fpv5-sp-d16

PRJL_FLOAT-ABI = -mfloat-abi=hard

PRJ_MCU = $(PRJ_CPU) -mthumb $(PRJ_FPU) $(PRJ_FLOAT-ABI)

PRJ_ALL_SOURCES = app/src \
hal/ulcgc/Firmware/GD32F5xx_standard_peripheral/Source \
hal/ulcgc/Firmware/GD32F5xx_usb_library/device/class/cdc/Source \
hal/ulcgc/Firmware/GD32F5xx_usb_library/device/core/Source \
hal/ulcgc/Firmware/GD32F5xx_usb_library/driver/Source \
hal/ulcgc/src	\
hal/ulcgc/Utilities/Source \
mid/src
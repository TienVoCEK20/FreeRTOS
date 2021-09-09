################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third-Party/FreeRTOS/org/Source/portable/MemMang/heap_4.c 

OBJS += \
./Third-Party/FreeRTOS/org/Source/portable/MemMang/heap_4.o 

C_DEPS += \
./Third-Party/FreeRTOS/org/Source/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
Third-Party/FreeRTOS/org/Source/portable/MemMang/%.o: ../Third-Party/FreeRTOS/org/Source/portable/MemMang/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F407VGTx -DSTM32F407G_DISC1 -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"D:/stm32_work/STM32_FreeRTOS_Led_Button_IR/StdPeriph_Driver/inc" -I"D:/stm32_work/STM32_FreeRTOS_Led_Button_IR/Config" -I"D:/stm32_work/STM32_FreeRTOS_Led_Button_IR/Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I"D:/stm32_work/STM32_FreeRTOS_Led_Button_IR/inc" -I"D:/stm32_work/STM32_FreeRTOS_Led_Button_IR/CMSIS/device" -I"D:/stm32_work/STM32_FreeRTOS_Led_Button_IR/CMSIS/core" -I"D:/stm32_work/STM32_FreeRTOS_Led_Button_IR/Third-Party/FreeRTOS/org/Source/include" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



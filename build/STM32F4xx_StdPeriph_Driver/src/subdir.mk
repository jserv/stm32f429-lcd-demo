# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
	../STM32F4xx_StdPeriph_Driver/src/misc.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sai.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
	../STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c

# Each subdirectory must supply rules for building sources it contributes
STM32F4xx_StdPeriph_Driver/src/%.o: ../STM32F4xx_StdPeriph_Driver/src/%.c
	@echo 'Building file: $<'
	arm-none-eabi-gcc -DSTM32F429_439xx -D__VFP_FP__ -DUSE_STDPERIPH_DRIVER -DUSE_STM32F429I_DISCO \
		-I ../base \
		-I ../User_HandsOn \
		-I ../STemWinLibrary522_4x9i/inc \
		-I ../STemWinLibrary522_4x9i/Config \
		-I ../lib \
		-I ../STM32F4xx_StdPeriph_Driver/inc \
		-O0 \
		-ffunction-sections -fdata-sections \
		-Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c \
		-fmessage-length=0 \
		-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" \
		-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

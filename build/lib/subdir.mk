# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
	../lib/fonts.c \
	../lib/stm32f429i_discovery.c \
	../lib/stm32f429i_discovery_ioe.c \
	../lib/stm32f429i_discovery_l3gd20.c \
	../lib/stm32f429i_discovery_lcd.c \
	../lib/stm32f429i_discovery_sdram.c 

OBJS += \
./lib/fonts.o \
./lib/stm32f429i_discovery.o \
./lib/stm32f429i_discovery_ioe.o \
./lib/stm32f429i_discovery_l3gd20.o \
./lib/stm32f429i_discovery_lcd.o \
./lib/stm32f429i_discovery_sdram.o 

C_DEPS += \
./lib/fonts.d \
./lib/stm32f429i_discovery.d \
./lib/stm32f429i_discovery_ioe.d \
./lib/stm32f429i_discovery_l3gd20.d \
./lib/stm32f429i_discovery_lcd.d \
./lib/stm32f429i_discovery_sdram.d 


# Each subdirectory must supply rules for building sources it contributes
lib/%.o: ../lib/%.c
	@echo 'Building file: $<'
	arm-none-eabi-gcc \
		-DSTM32F429_439xx \
		-D__VFP_FP__ \
		-DUSE_STDPERIPH_DRIVER \
		-DUSE_STM32F429I_DISCO \
		-I ../base \
		-I ../User_HandsOn \
		-I ../STemWinLibrary522_4x9i/inc \
		-I ../STemWinLibrary522_4x9i/Config \
		-I ../lib \
		-I ../STM32F4xx_StdPeriph_Driver/inc \
		-O0 \
		-ffunction-sections -fdata-sections \
		-Wall -std=gnu99 \
		-Wa,-adhlns="$@.lst" \
		-c \
		-fmessage-length=0 \
		-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" \
		-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

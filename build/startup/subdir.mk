# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../startup/startup_stm32f429_439xx.S 

OBJS += \
./startup/startup_stm32f429_439xx.o 

S_UPPER_DEPS += \
./startup/startup_stm32f429_439xx.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.S
	@echo 'Building file: $<'
	arm-none-eabi-gcc -x assembler-with-cpp \
		-DSTM32F429_439xx -D__VFP_FP__ -DUSE_STM32F429I_DISCO \
		-I ../startup \
		-Wa,-adhlns="$@.lst" -c -fmessage-length=0 \
		-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" \
		-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

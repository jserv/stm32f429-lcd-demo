RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include lib/subdir.mk
-include base/subdir.mk
-include User_HandsOn/subdir.mk
-include STemWinLibrary522_4x9i/OS/subdir.mk
-include STemWinLibrary522_4x9i/Config/subdir.mk
-include STM32F4xx_StdPeriph_Driver/src/subdir.mk
-include subdir.mk
-include objects.mk

OBJS += \
	$(C_SRCS:../%.c=%.o) $(S_UPPER_SRCS:../%.S=%.o)

C_DEPS += \
	$(C_SRCS:../%.c=%.d)

S_UPPER_DEPS += \
	$(S_UPPER_SRCS:../%.S=%.d)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
	stm32f429_lcd2.hex \
	stm32f429_lcd2.bin

SECONDARY_LIST += \
stm32f429_lcd2.lst \

# All Target
all: stm32f429_lcd2.elf secondary-outputs

# Tool invocations
stm32f429_lcd2.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	arm-none-eabi-gcc \
		-T ../startup/STM32F429ZI_FLASH.ld \
		-nostartfiles -Xlinker --gc-sections \
		-L ../STemWinLibrary522_4x9i/Lib \
		-Wl,-Map,stm32f429_lcd2.map -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -o "stm32f429_lcd2.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

stm32f429_lcd2.hex: stm32f429_lcd2.elf
	arm-none-eabi-objcopy -O ihex stm32f429_lcd2.elf  "stm32f429_lcd2.hex"
	@echo 'Finished building: $@'
	@echo ' '

stm32f429_lcd2.bin: stm32f429_lcd2.elf
	arm-none-eabi-objcopy -O binary stm32f429_lcd2.elf  "stm32f429_lcd2.bin"
	@echo 'Finished building: $@'
	@echo ' '

stm32f429_lcd2.lst: stm32f429_lcd2.elf
	arm-none-eabi-objdump -h -S stm32f429_lcd2.elf > "stm32f429_lcd2.lst"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) \
		$(OBJS) $(C_DEPS) $(ASM_DEPS) \
		$(SECONDARY_FLASH) $(EXECUTABLES) $(SECONDARY_LIST) $(S_UPPER_DEPS) stm32f429_lcd2.elf
	-@echo ' '
	find -name '*.lst' | xargs $(RM)
	$(RM) stm32f429_lcd2.map

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

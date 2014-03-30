# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
	../User_HandsOn/ImageRGB888.c \
	../User_HandsOn/ImageSTLogo.c \
	../User_HandsOn/RS389_Cover_240.c \
	../User_HandsOn/Style-Next-icon.c \
	../User_HandsOn/Style-Pause-icon.c \
	../User_HandsOn/Style-Play-icon.c \
	../User_HandsOn/Style-Previous-icon.c \
	../User_HandsOn/Style-Stop-icon.c \
	../User_HandsOn/Times_New_Roman31.c \
	../User_HandsOn/Verdana32.c \
	../User_HandsOn/background.c \
	../User_HandsOn/bsp.c \
	../User_HandsOn/main.c \
	../User_HandsOn/main_2D.c \
	../User_HandsOn/main_2D_AAtext.c \
	../User_HandsOn/main_2D_text.c \
	../User_HandsOn/main_2D_text_bmp.c \
	../User_HandsOn/main_2layers.c \
	../User_HandsOn/main_Graph.c \
	../User_HandsOn/main_Gyro.c \
	../User_HandsOn/main_WM_button.c \
	../User_HandsOn/main_WM_button_list.c \
	../User_HandsOn/main_WM_button_list_icons.c \
	../User_HandsOn/main_WM_button_skin.c \
	../User_HandsOn/main_colorFormat.c \
	../User_HandsOn/main_colorKeying.c \
	../User_HandsOn/main_contentCreation.c \
	../User_HandsOn/main_layerScroll.c \
	../User_HandsOn/stm32xxx_it.c \
	../User_HandsOn/timeMeasure.c 

# Each subdirectory must supply rules for building sources it contributes
User_HandsOn/%.o: ../User_HandsOn/%.c
	@echo 'Building file: $<'
	arm-none-eabi-gcc -DSTM32F429_439xx -D__VFP_FP__ -DUSE_STDPERIPH_DRIVER -DUSE_STM32F429I_DISCO \
		-I ../base -I ../User_HandsOn \
		-I ../STemWinLibrary522_4x9i/inc \
		-I ../STemWinLibrary522_4x9i/Config \
		-I ../lib \
		-I ../STM32F4xx_StdPeriph_Driver/inc \
		-O0 -ffunction-sections -fdata-sections \
		-Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 \
		-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

User_HandsOn/Times_New_Roman31.o: ../User_HandsOn/Times_New_Roman31.c
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
		-Wall -std=gnu99 -Wa,-adhlns="$@.lst" \
		-c \
		-fmessage-length=0 \
		-MMD -MP -MF"User_HandsOn/Times_New_Roman31.d" \
		-MT"User_HandsOn/Times_New_Roman31.d" \
		-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

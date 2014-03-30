all:
	$(MAKE) -C build all

clean:
	$(MAKE) -C build clean

flash: build/stm32f429_lcd2.bin
	st-flash write build/stm32f429_lcd2.bin 0x8000000

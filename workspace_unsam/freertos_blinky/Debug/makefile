################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include freertos/src/subdir.mk
-include example/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: freertos_blinky.axf

# Tool invocations
freertos_blinky.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\CMSIS_DSPLIB_CM4\lib" -L"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\lpc_chip_43xx\Debug" -L"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\lpc_board_nxp_lpcxpresso_4337\Debug" -Xlinker -Map="freertos_blinky.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "freertos_blinky_Debug.ld" -o "freertos_blinky.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) freertos_blinky.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "freertos_blinky.axf"; # arm-none-eabi-objcopy -v -O binary "freertos_blinky.axf" "freertos_blinky.bin" ; # checksum -p LPC4337 -d "freertos_blinky.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets

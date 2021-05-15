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
all: 11.\ freertos_queue2.axf

# Tool invocations
11.\ freertos_queue2.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\lenovo\Documents\MCUXpressoIDE_11.1.1_3241\workspace_unsam\lpc_chip_43xx\Debug" -L"C:\Users\lenovo\Documents\MCUXpressoIDE_11.1.1_3241\workspace_unsam\lpc_board_nxp_lpcxpresso_4337\Debug" -Xlinker -Map="11. freertos_queue2.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "11_freertos_queue2_Debug.ld" -o "11. freertos_queue2.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) "11. freertos_queue2.axf"
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "11. freertos_queue2.axf"; # arm-none-eabi-objcopy -v -O binary "11. freertos_queue2.axf" "11. freertos_queue2.bin" ; # checksum -p LPC4337 -d "11. freertos_queue2.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets

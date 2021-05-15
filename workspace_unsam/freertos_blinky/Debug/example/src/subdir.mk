################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/blinky.c \
../example/src/cr_startup_lpc43xx.c \
../example/src/sysinit.c 

OBJS += \
./example/src/blinky.o \
./example/src/cr_startup_lpc43xx.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/blinky.d \
./example/src/cr_startup_lpc43xx.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__MULTICORE_NONE -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -D__REDLIB__ -DCORE_M4 -I"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\CMSIS_DSPLIB_CM4\inc" -I"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\lpc_chip_43xx\inc" -I"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\lpc_board_nxp_lpcxpresso_4337\inc" -I"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\freertos_blinky\example\inc" -I"C:\Users\Juan\Documents\MCUXpressoIDE_11.1.1_3241\workspace\freertos_blinky\freertos\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



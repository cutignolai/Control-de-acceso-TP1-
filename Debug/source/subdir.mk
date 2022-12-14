################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/App.c \
../source/SysTick.c \
../source/button.c \
../source/card_reader.c \
../source/display.c \
../source/encoder.c \
../source/gpio.c \
../source/leds.c \
../source/timer.c \
../source/user.c 

OBJS += \
./source/App.o \
./source/SysTick.o \
./source/button.o \
./source/card_reader.o \
./source/display.o \
./source/encoder.o \
./source/gpio.o \
./source/leds.o \
./source/timer.o \
./source/user.o 

C_DEPS += \
./source/App.d \
./source/SysTick.d \
./source/button.d \
./source/card_reader.d \
./source/display.d \
./source/encoder.d \
./source/gpio.d \
./source/leds.d \
./source/timer.d \
./source/user.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



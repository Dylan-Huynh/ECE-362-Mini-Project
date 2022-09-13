################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BG_GameOver.c \
../src/GObottomText.c \
../src/GOtext.c \
../src/astroid.c \
../src/background.c \
../src/ball.c \
../src/explosion.c \
../src/lcd.c \
../src/main.c \
../src/rubble_1.c \
../src/rubble_2.c \
../src/support.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

OBJS += \
./src/BG_GameOver.o \
./src/GObottomText.o \
./src/GOtext.o \
./src/astroid.o \
./src/background.o \
./src/ball.o \
./src/explosion.o \
./src/lcd.o \
./src/main.o \
./src/rubble_1.o \
./src/rubble_2.o \
./src/support.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

C_DEPS += \
./src/BG_GameOver.d \
./src/GObottomText.d \
./src/GOtext.d \
./src/astroid.d \
./src/background.d \
./src/ball.d \
./src/explosion.d \
./src/lcd.d \
./src/main.d \
./src/rubble_1.d \
./src/rubble_2.d \
./src/support.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"/home/shay/a/patton37/Desktop/Mini_Project/StdPeriph_Driver/inc" -I"/home/shay/a/patton37/Desktop/Mini_Project/inc" -I"/home/shay/a/patton37/Desktop/Mini_Project/CMSIS/device" -I"/home/shay/a/patton37/Desktop/Mini_Project/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



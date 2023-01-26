################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BG_GameOver.c \
../src/Dire-Dire-Docks.c \
../src/GObottomText.c \
../src/GOtext.c \
../src/Game-Over.c \
../src/astroid.c \
../src/background.c \
../src/ball.c \
../src/explosion.c \
../src/lcd.c \
../src/main.c \
../src/midi.c \
../src/numbers.c \
../src/obstacles.c \
../src/rubble_1.c \
../src/rubble_2.c \
../src/score.c \
../src/step.c \
../src/support.c \
../src/syscalls.c \
../src/system_stm32f0xx.c \
../src/wavetable.c 

OBJS += \
./src/BG_GameOver.o \
./src/Dire-Dire-Docks.o \
./src/GObottomText.o \
./src/GOtext.o \
./src/Game-Over.o \
./src/astroid.o \
./src/background.o \
./src/ball.o \
./src/explosion.o \
./src/lcd.o \
./src/main.o \
./src/midi.o \
./src/numbers.o \
./src/obstacles.o \
./src/rubble_1.o \
./src/rubble_2.o \
./src/score.o \
./src/step.o \
./src/support.o \
./src/syscalls.o \
./src/system_stm32f0xx.o \
./src/wavetable.o 

C_DEPS += \
./src/BG_GameOver.d \
./src/Dire-Dire-Docks.d \
./src/GObottomText.d \
./src/GOtext.d \
./src/Game-Over.d \
./src/astroid.d \
./src/background.d \
./src/ball.d \
./src/explosion.d \
./src/lcd.d \
./src/main.d \
./src/midi.d \
./src/numbers.d \
./src/obstacles.d \
./src/rubble_1.d \
./src/rubble_2.d \
./src/score.d \
./src/step.d \
./src/support.d \
./src/syscalls.d \
./src/system_stm32f0xx.d \
./src/wavetable.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"/home/shay/a/huynh38/workspace/MiniProjectFinal3/StdPeriph_Driver/inc" -I"/home/shay/a/huynh38/workspace/MiniProjectFinal3/inc" -I"/home/shay/a/huynh38/workspace/MiniProjectFinal3/CMSIS/device" -I"/home/shay/a/huynh38/workspace/MiniProjectFinal3/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



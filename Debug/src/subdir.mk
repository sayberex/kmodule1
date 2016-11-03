################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/kmodule1.c \
../src/kmodule1.mod.c 

O_SRCS += \
../src/kmodule1.mod.o \
../src/kmodule1.o 

OBJS += \
./src/kmodule1.o \
./src/kmodule1.mod.o 

C_DEPS += \
./src/kmodule1.d \
./src/kmodule1.mod.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/src/linux-headers-3.16.0-4-common/include -I/usr/src/linux-headers-3.16.0-4-common/arch/x86/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



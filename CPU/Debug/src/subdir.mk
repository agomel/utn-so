################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CPU.c \
../src/comunicaciones.c \
../src/operaciones.c \
../src/parser.c 

OBJS += \
./src/CPU.o \
./src/comunicaciones.o \
./src/operaciones.o \
./src/parser.o 

C_DEPS += \
./src/CPU.d \
./src/comunicaciones.d \
./src/operaciones.d \
./src/parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MDJ.c \
../src/comandos.c \
../src/consola.c \
../src/operaciones.c 

OBJS += \
./src/MDJ.o \
./src/comandos.o \
./src/consola.o \
./src/operaciones.o 

C_DEPS += \
./src/MDJ.d \
./src/comandos.d \
./src/consola.d \
./src/operaciones.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



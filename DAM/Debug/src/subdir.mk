################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/DAM.c \
../src/ServicioFM9.c \
../src/ServicioMDJ.c \
../src/ServicioSAFA.c 

OBJS += \
./src/DAM.o \
./src/ServicioFM9.o \
./src/ServicioMDJ.o \
./src/ServicioSAFA.o 

C_DEPS += \
./src/DAM.d \
./src/ServicioFM9.d \
./src/ServicioMDJ.d \
./src/ServicioSAFA.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



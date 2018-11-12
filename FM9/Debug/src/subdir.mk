################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FM9.c \
../src/manejoLineas.c \
../src/paginasInvertidas.c \
../src/segmentacionPag.c \
../src/segmentacionPura.c 

OBJS += \
./src/FM9.o \
./src/manejoLineas.o \
./src/paginasInvertidas.o \
./src/segmentacionPag.o \
./src/segmentacionPura.o 

C_DEPS += \
./src/FM9.d \
./src/manejoLineas.d \
./src/paginasInvertidas.d \
./src/segmentacionPag.d \
./src/segmentacionPura.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



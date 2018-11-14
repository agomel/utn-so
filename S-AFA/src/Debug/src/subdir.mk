################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/S-AFA.c \
../src/consola.c \
../src/gestorDeDTBs.c \
../src/listas.c \
../src/pcp.c \
../src/planificador.c \
../src/plp.c 

OBJS += \
./src/S-AFA.o \
./src/consola.o \
./src/gestorDeDTBs.o \
./src/listas.o \
./src/pcp.o \
./src/planificador.o \
./src/plp.o 

C_DEPS += \
./src/S-AFA.d \
./src/consola.d \
./src/gestorDeDTBs.d \
./src/listas.d \
./src/pcp.d \
./src/planificador.d \
./src/plp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



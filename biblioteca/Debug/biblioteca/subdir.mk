################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../biblioteca/dtb.c \
../biblioteca/hilos.c \
../biblioteca/logger.c \
../biblioteca/nuestroSelect.c \
../biblioteca/select.c \
../biblioteca/semaforos.c \
../biblioteca/serializacion.c \
../biblioteca/socket.c \
../biblioteca/traductor.c \
../biblioteca/utilidades.c 

OBJS += \
./biblioteca/dtb.o \
./biblioteca/hilos.o \
./biblioteca/logger.o \
./biblioteca/nuestroSelect.o \
./biblioteca/select.o \
./biblioteca/semaforos.o \
./biblioteca/serializacion.o \
./biblioteca/socket.o \
./biblioteca/traductor.o \
./biblioteca/utilidades.o 

C_DEPS += \
./biblioteca/dtb.d \
./biblioteca/hilos.d \
./biblioteca/logger.d \
./biblioteca/nuestroSelect.d \
./biblioteca/select.d \
./biblioteca/semaforos.d \
./biblioteca/serializacion.d \
./biblioteca/socket.d \
./biblioteca/traductor.d \
./biblioteca/utilidades.d 


# Each subdirectory must supply rules for building sources it contributes
biblioteca/%.o: ../biblioteca/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



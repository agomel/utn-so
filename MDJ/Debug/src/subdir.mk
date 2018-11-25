################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MDJ.c \
../src/bitmap.c \
../src/bloques.c \
../src/comandos.c \
../src/consola.c \
../src/fileSystem.c \
../src/operacionesArchivos.c \
../src/operacionesFIFA.c \
../src/parser.c \
../src/pruebas.c 

OBJS += \
./src/MDJ.o \
./src/bitmap.o \
./src/bloques.o \
./src/comandos.o \
./src/consola.o \
./src/fileSystem.o \
./src/operacionesArchivos.o \
./src/operacionesFIFA.o \
./src/parser.o \
./src/pruebas.o 

C_DEPS += \
./src/MDJ.d \
./src/bitmap.d \
./src/bloques.d \
./src/comandos.d \
./src/consola.d \
./src/fileSystem.d \
./src/operacionesArchivos.d \
./src/operacionesFIFA.d \
./src/parser.d \
./src/pruebas.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



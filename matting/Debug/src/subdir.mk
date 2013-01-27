################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gui.cpp \
../src/main.cpp \
../src/matting.cpp \
../src/solver.cpp 

OBJS += \
./src/gui.o \
./src/main.o \
./src/matting.o \
./src/solver.o 

CPP_DEPS += \
./src/gui.d \
./src/main.d \
./src/matting.d \
./src/solver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include -I/opt/intel/composerxe/mkl/include -I/opt/mtl/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



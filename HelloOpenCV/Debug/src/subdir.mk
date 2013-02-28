################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DetectCircle.cpp \
../src/DetectLine.cpp \
../src/DetectRectangle.cpp \
../src/HelloOpenCV.cpp \
../src/TestHZ.cpp 

OBJS += \
./src/DetectCircle.o \
./src/DetectLine.o \
./src/DetectRectangle.o \
./src/HelloOpenCV.o \
./src/TestHZ.o 

CPP_DEPS += \
./src/DetectCircle.d \
./src/DetectLine.d \
./src/DetectRectangle.d \
./src/HelloOpenCV.d \
./src/TestHZ.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



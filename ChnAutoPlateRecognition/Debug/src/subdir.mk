################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CvxText.cpp \
../src/DetectRectangle.cpp \
../src/DetectRegions.cpp \
../src/OCR.cpp \
../src/Plate.cpp \
../src/evalOCR.cpp \
../src/main.cpp \
../src/trainOCR.cpp \
../src/trainSVM.cpp 

OBJS += \
./src/CvxText.o \
./src/DetectRectangle.o \
./src/DetectRegions.o \
./src/OCR.o \
./src/Plate.o \
./src/evalOCR.o \
./src/main.o \
./src/trainOCR.o \
./src/trainSVM.o 

CPP_DEPS += \
./src/CvxText.d \
./src/DetectRectangle.d \
./src/DetectRegions.d \
./src/OCR.d \
./src/Plate.d \
./src/evalOCR.d \
./src/main.d \
./src/trainOCR.d \
./src/trainSVM.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include -I/usr/include/freetype2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



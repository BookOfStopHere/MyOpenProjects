################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CvxText.cpp \
../src/DetectCircle.cpp \
../src/DetectLine.cpp \
../src/DetectPeople.cpp \
../src/DetectRectangle.cpp \
../src/Geometric_Transforms_Demo.cpp \
../src/HelloOpenCV.cpp \
../src/PlayVideo.cpp \
../src/PrintFilesInDir.cpp \
../src/ResizeImage.cpp \
../src/TestCvx.cpp \
../src/TestHZ.cpp \
../src/WriteVideo.cpp \
../src/calcBackProject_Demo2.cpp 

OBJS += \
./src/CvxText.o \
./src/DetectCircle.o \
./src/DetectLine.o \
./src/DetectPeople.o \
./src/DetectRectangle.o \
./src/Geometric_Transforms_Demo.o \
./src/HelloOpenCV.o \
./src/PlayVideo.o \
./src/PrintFilesInDir.o \
./src/ResizeImage.o \
./src/TestCvx.o \
./src/TestHZ.o \
./src/WriteVideo.o \
./src/calcBackProject_Demo2.o 

CPP_DEPS += \
./src/CvxText.d \
./src/DetectCircle.d \
./src/DetectLine.d \
./src/DetectPeople.d \
./src/DetectRectangle.d \
./src/Geometric_Transforms_Demo.d \
./src/HelloOpenCV.d \
./src/PlayVideo.d \
./src/PrintFilesInDir.d \
./src/ResizeImage.d \
./src/TestCvx.d \
./src/TestHZ.d \
./src/WriteVideo.d \
./src/calcBackProject_Demo2.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include -I/usr/include/freetype2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



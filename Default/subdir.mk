################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Block.cpp \
../BlockMaker.cpp \
../FftTransform.cpp \
../MelAnalyzer.cpp \
../MfccSettings.cpp \
../Pusher.cpp \
../main.cpp \
../vq.cpp 

OBJS += \
./Block.o \
./BlockMaker.o \
./FftTransform.o \
./MelAnalyzer.o \
./MfccSettings.o \
./Pusher.o \
./main.o \
./vq.o 

CPP_DEPS += \
./Block.d \
./BlockMaker.d \
./FftTransform.d \
./MelAnalyzer.d \
./MfccSettings.d \
./Pusher.d \
./main.d \
./vq.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/boost/lockfree -I/usr/include/boost -I/usr/include/boost/thread -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



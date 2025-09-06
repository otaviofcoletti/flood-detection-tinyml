# Tool macros
CC = gcc
CXX = g++

# Settings
NAME = app
BUILD_PATH = build

# OpenCV configuration
OPENCV_FLAGS = $(shell pkg-config --cflags opencv4)
OPENCV_LIBS = $(shell pkg-config --libs opencv4)

# Compiler flags
CFLAGS = -I. -Wall -g -Wno-strict-aliasing -Os -DNDEBUG -DEI_CLASSIFIER_ENABLE_DETECTION_POSTPROCESS_OP
CXXFLAGS = -std=c++14 $(OPENCV_FLAGS)
LDFLAGS = -lm -lstdc++ $(OPENCV_LIBS)

# Source files
CXXSOURCES = main.cpp
CSOURCES = $(wildcard edge-impulse-sdk/CMSIS/DSP/Source/*/*.c)
CCSOURCES = $(wildcard edge-impulse-sdk/tensorflow/lite/*/*.cc) \
            $(wildcard edge-impulse-sdk/tensorflow/lite/*/*/*.cc) \
            $(wildcard edge-impulse-sdk/tensorflow/lite/*/*/*/*.cc)

CXXSOURCES += $(wildcard tflite-model/*.cpp) \
              $(wildcard edge-impulse-sdk/dsp/*.cpp) \
              $(wildcard edge-impulse-sdk/dsp/*/*.cpp) \
              $(wildcard edge-impulse-sdk/porting/*/*.cpp)

# Object files
COBJECTS = $(CSOURCES:.c=.o)
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CCOBJECTS = $(CCSOURCES:.cc=.o)

# Default target
all: app

# Link the application
app: $(COBJECTS) $(CXXOBJECTS) $(CCOBJECTS)
	@mkdir -p $(BUILD_PATH)
	$(CXX) $^ -o $(BUILD_PATH)/$(NAME) $(LDFLAGS)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ files
%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

# Compile CC files
%.o: %.cc
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(COBJECTS) $(CXXOBJECTS) $(CCOBJECTS)
	rm -f $(BUILD_PATH)/$(NAME)

.PHONY: all app clean
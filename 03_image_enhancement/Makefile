# Simple Makefile

CXX = g++
CXXFLAGS = -Wall -g -std=c++11

LIBS = /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 /usr/lib64/libopencv_imgproc.so.2.4

TARGET = main.out

# SRC_DIRS ?= ./src
# SRCS := $(shell find $(SRC_DIRS) -name *.cpp)

EXEC = deconvolution convolution filter2d

MKDIRS = mkdir -p out

all: mkdirs $(EXEC)
	
mkdirs:
	$(MKDIRS)

deconvolution:
	$(CXX) $(CXXFLAGS) $(LIBS) -o deconvolution.out src/deconvolution.cpp

convolution:
	$(CXX) $(CXXFLAGS) $(LIBS) -o convolution.out src/convolution.cpp

filter2d:
	$(CXX) $(CXXFLAGS) $(LIBS) -o filter2d.out src/filter2d.cpp

.PHONY: clean all
clean:
	\rm -f deconvolution.out
	\rm -f convolution.out
	\rm -f filter2d.out
	\rm -f -r out
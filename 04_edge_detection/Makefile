# Simple Makefile for Edges project

CXX = g++
CXXFLAGS = -Wall -g -std=c++11

LIBS = /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 /usr/lib64/libopencv_imgproc.so.2.4

TARGET = main.out
# SRC   = src/main.cpp src/gaussianBlur.cpp src/houghCircles.cpp src/sobelEdges.cpp src/utils.cpp

SRC_DIRS ?= ./src
SRCS := $(shell find $(SRC_DIRS) -name *.cpp)

MKDIRS = mkdir -p out

main:
	\$(MKDIRS)
	\$(CXX) $(CXXFLAGS) $(LIBS) -o $(TARGET) $(SRCS)

.PHONY: clean all
clean:
	\rm -f main.out
	\rm -f -r out
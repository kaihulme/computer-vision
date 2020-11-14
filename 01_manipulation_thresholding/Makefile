# Simple Makefile

CXX = g++
CXXFLAGS = -Wall -g -std=c++11

LIBS = /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 /usr/lib64/libopencv_imgproc.so.2.4

EXEC = display draw hello pixels thr

MKDIRS = mkdir -p out

all: mkdirs $(EXEC)

mkdirs:
	$(MKDIRS)
	
display:
	$(CXX) $(CXXFLAGS) $(LIBS) -o display.out src/display.cpp

draw:
	$(CXX) $(CXXFLAGS) $(LIBS) -o draw.out src/draw.cpp

hello:
	$(CXX) $(CXXFLAGS) $(LIBS) -o hello.out src/hello.cpp

pixels:
	$(CXX) $(CXXFLAGS) $(LIBS) -o pixels.out src/pixels.cpp

thr:
	$(CXX) $(CXXFLAGS) $(LIBS) -o thr.out src/thr.cpp

.PHONY: clean all
clean:
	\rm -f display.out
	\rm -f draw.out
	\rm -f hello.out
	\rm -f pixels.out
	\rm -f thr.out
	\rm -f -r out
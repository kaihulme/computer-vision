# Simple Makefile

CXX = g++
CXXFLAGS = -Wall -g -std=c++11

LIBS = /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 /usr/lib64/libopencv_imgproc.so.2.4

EXEC = colourthr convolution image0_fix image1_fix image2_fix image3_fix RGBtoGRAY.cpp RGBtoHSV.cpp

MKDIRS = mkdir -p out

all: mkdirs $(EXEC)
	
mkdirs:
	$(MKDIRS)
	
colourthr:
	$(CXX) $(CXXFLAGS) $(LIBS) -o colourthr.out src/colourthr.cpp

convolution:
	$(CXX) $(CXXFLAGS) $(LIBS) -o convolution.out src/convolution.cpp

image0_fix:
	$(CXX) $(CXXFLAGS) $(LIBS) -o image0_fix.out src/image0_fix.cpp

image1_fix:
	$(CXX) $(CXXFLAGS) $(LIBS) -o image1_fix.out src/image1_fix.cpp

image2_fix:
	$(CXX) $(CXXFLAGS) $(LIBS) -o image2_fix.out src/image2_fix.cpp

image3_fix:
	$(CXX) $(CXXFLAGS) $(LIBS) -o image3_fix.out src/image3_fix.cpp

RGBtoGRAY:
	$(CXX) $(CXXFLAGS) $(LIBS) -o RGBtoGRAY.out src/RGBtoGRAY.cpp

RGBtoHSV:
	$(CXX) $(CXXFLAGS) $(LIBS) -o RGBtoHSV.out src/RGBtoHSV.cpp

.PHONY: clean all
clean:
	\rm -f colourthr.out
	\rm -f convolution.out
	\rm -f image0_fix.out
	\rm -f image1_fix.out
	\rm -f image2_fix.out
	\rm -f image3_fix.out
	\rm -f RGBtoGRAY.out
	\rm -f RGBtoHSV.out
	\rm -f -r out
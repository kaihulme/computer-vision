# Make C++

.PHONY = all clean

CXX = g++
LINKERFLAG = -lm

LIBS = "/usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4"

SRCS := $(wildcard *.cpp)
BINS := $(SRCS:%.cpp=%)


all: ${BINS}

# %: %.out
# 	@echo "Checking.."
# 	${CC} ${LINKERFLAG} $< -out $@

# %.out: %.cpp
# 	@echo "Creating object.."
# 	${CC} -g /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 $<

%.out: %.cpp
	@echo "$(CXX) -g $< /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 -o $@"
	$(CXX) -g $< /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 -o $@
	# $(CXX) -g test.cpp /usr/lib64/libopencv_core.so.2.4 /usr/lib64/libopencv_highgui.so.2.4 -o test

clean:
	@echo "Cleaning up..."
	rm -rvf *.out ${BINS}

# hello: hello/hello.cpp
# 	${cc} hello/hello.cpp libs -o hello

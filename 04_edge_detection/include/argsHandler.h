#include <stdio.h>
#include <string>
#include <include/utils.h>

void ArgsHandler(int argc, char *argv[],
                 bool &sobel, bool &hough_circles, 
                 bool &threshold, bool &gaussian,
	             int &gaussian_val, int &threshold_val,
	             int &min_r, int &max_r,
	             int &r_step, int &t_step);
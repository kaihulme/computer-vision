#include <include/argsHandler.h>

void ArgsHandler(int argc, char *argv[],
                 bool &sobel, bool &hough_circles, 
                 bool &threshold, bool &gaussian,
	             int &gaussian_val, int &threshold_val,
	             int &min_r, int &max_r,
	             int &r_step, int &t_step) {

    // set default values
    sobel, hough_circles, threshold, gaussian = false;
	gaussian_val = 3;
    threshold_val = 100;
	min_r = 5;
    max_r = 50;
	r_step = 1;
    t_step = 1;

}
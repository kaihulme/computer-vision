#include <stdio.h>
#include <string>
#include <fstream>

#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h> 

#include <include/utils.h>

int ArgsHandler(int argc, char *argv[], 
                cv::Mat &image, string &image_name,
                bool &sobel, bool &hough_circles, 
                bool &threshold, bool &gaussian,
	            int &gaussian_val, int &threshold_val,
	            int &min_r, int &max_r,
	            int &r_step, int &t_step,
                int &threshold_h);

void ArgsHelper();
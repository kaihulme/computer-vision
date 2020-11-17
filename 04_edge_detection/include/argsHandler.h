#include <string>
#include <fstream>

#include <stdio.h>
#include <opencv/cv.h>        
#include <opencv/cxcore.h> 
#include <opencv/highgui.h>   

#include <include/utils.h>

// handles command line arguments
int ArgsHandler(int     argc,               // no. of arguments
                char    *argv[],            // arguments
                cv::Mat &image,             // image
                string  &image_name,        // image name
                bool    &sobel,             // whether or not to apply sobel edge detection
                bool    &hough_circles,     // whether or not to apply hough transform (circles)
                bool    &threshold,         // whether or not to apply magnitude thresholding
                bool    &gaussian,          // whether or not to apply gaussian smoothing
	            int     &gaussian_val,      // gaussian kernel size
                int     &threshold_val,     // threshold for sobel gradient magnitude
	            int     &min_r,             // min radius for hough circles
                int     &max_r,             // max radius for hough circles
	            int     &r_step,            // radius stepping for hough circles
                int     &t_step,            // theta stepping for hough circles
                int     &threshold_h);      // hough space threshold

// writes contents of args_help.txt
void ArgsHelper();
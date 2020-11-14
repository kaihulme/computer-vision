#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h> 

using namespace cv;

void GaussianBlur(const cv::Mat &input, const int size,
	              cv::Mat &gaussian_output);
#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>

using namespace cv;

void HoughTransformCircles(const cv::Mat &input, int r_size,
				     	   int min_r, int max_r, int r_step, int t_step,
				     	   std::vector<cv::Mat> &hough_circles_space);
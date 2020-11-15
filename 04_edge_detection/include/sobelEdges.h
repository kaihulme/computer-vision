#include <stdio.h>
#include <string>
#include <iostream>

#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>

using namespace cv;

void SobelEdgeDetector(const cv::Mat &input, const int size, 
					   cv::Mat &dfdx_output, cv::Mat &dfdy_output,
					   cv::Mat &maginitude_output, cv::Mat &direction_output);
#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>

using namespace cv;

double Convolution(cv::Mat &input, cv::Mat &kernel, 
                   int i, int j, const int r_x, const int r_y);

void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output);

void NormalisePixels(cv::Mat &input, cv::Mat &output);

void NormaliseWrite(string img_name, string out_type, int arg, cv::Mat output);

void RadToDeg(cv::Mat &input, cv::Mat &output);
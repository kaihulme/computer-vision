#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>

using namespace cv;

void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output);

void NormalisePixels(cv::Mat &input, cv::Mat &output);

void NormaliseWrite(string img_name, string out_type, int arg, cv::Mat output);

void RadToDeg(cv::Mat &input, cv::Mat &output);

void ArgsHelper();
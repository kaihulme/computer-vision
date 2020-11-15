#include <stdio.h>
#include <string>
#include <iostream>

#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>

using namespace cv;

#ifndef UTILS_H
#define UTILS_H

struct pos{ 
    int r; 
    int x; 
    int y; 
};

#endif /* UTILS_H */

double Convolution(cv::Mat &input, cv::Mat &kernel, 
                   int i, int j, const int r_x, const int r_y);

void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output);

pos FindLocalMaxima(std::vector<cv::Mat> &space,
                    int r, int x, int y, 
	                int local_size);

void DrawCircles(cv::Mat image, std::vector<pos>);

void NormalisePixels(cv::Mat &input, cv::Mat &output);

void NormaliseWrite(string img_name, string out_type, int arg, cv::Mat output);

void RadToDeg(cv::Mat &input, cv::Mat &output);

int FitToSpace(int x, int max);
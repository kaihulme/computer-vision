#include <stdio.h>
#include <string>
#include <iostream>
#include <string>

#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>

using namespace cv;

#ifndef UTILS_H
#define UTILS_H

    struct pos_t {
        int x;
        int y;
    };

    struct circle_t {
        pos_t pos;
        int radius;
    };

    struct w_pos_t {
        pos_t    pos;
        double weight;
    };


#endif /* UTILS_H */

double Convolution(cv::Mat &input, cv::Mat &kernel, 
                   int i, int j, const int r_x, const int r_y);

void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output);

pos_t FindLocalMaxima(std::vector<cv::Mat> &hough_space_circles,
                    int r, int x, int y, 
	                int local_size);

pos_t LocalMax(cv::Mat &space, int x, int y, int border,
			   int region_size);

void DrawCircles(cv::Mat &image, std::vector<circle_t> &circles, 
                 std::string image_name);

void NormalisePixels(cv::Mat &input, cv::Mat &output);

void NormaliseWrite(string img_name, string out_type, int arg, cv::Mat output);

void RadToDeg(cv::Mat &input, cv::Mat &output);

int FitToSpace(int x, int max);
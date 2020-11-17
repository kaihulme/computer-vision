#include <stdio.h>
#include <string>
#include <iostream>

#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>

#include <include/utils.h>
#include <include/houghCircles.h>

using namespace cv;

// find circles in img
std::vector<circle_t> FindCircles(std::vector<cv::Mat> &hough_space,
                                  cv::Mat &hough_space_sum,
								  int r_size, int min_r, int r_step);
	
// return circle locations as local maxima in img
std::vector<pos_t> GetCircleLocs(cv::Mat img);

// remove any circles closer than dist
void RemoveCloseNeighbours(std::vector<w_pos_t> &circle_locs, int dist);

// removes a circle position of given value
void RemoveCircle(w_pos_t circle, std::vector<w_pos_t> &sparse_locs);
			
// recursively heads towards the best neighbour
pos_t GetBestNeighbour(cv::Mat img, pos_t pos, double curr);

// calculate distance between two points
double CalcDist(pos_t a, pos_t b);

// checks two weighed positions are not equal
bool PosEqual(pos_t a, pos_t b);
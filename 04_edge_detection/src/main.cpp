
#ifndef UTILS_H_
#define UTILS_H_

	// utils.h code

#endif

#include <include/gaussianBlur.h>
#include <include/sobelEdges.h>
#include <include/houghCircles.h>
#include <include/argsHandler.h>

int main(int argc, char* argv[]) {

	// image from file
	cv::Mat img_input;
	string image_name;

	// variables for arguments
	bool sobel; bool hough_circles; 
    bool threshold; bool gaussian;
	int gaussian_val; int threshold_val;
	int min_r; int max_r;
	int r_step; int t_step;

	// get image, set argument variables and return -1 if invalid
	int pass = ArgsHandler(argc, argv, img_input, image_name,
                           sobel, hough_circles, 
                           threshold, gaussian,
	                       gaussian_val, threshold_val,
	                       min_r, max_r,
	                       r_step, t_step);
	if (pass == -1) return -1;

	// apply gaussian filter
	if (gaussian) { 
		cv::Mat img_gaussian;
		GaussianBlur(img_input, gaussian_val, img_gaussian);
		NormaliseWrite(image_name, "gaussian", 0, img_gaussian);
		img_input = img_gaussian;
	}
	
	// sobel edge detector
    if (sobel) {
	
		// set kernel size and output matrices
        const int kernel_size = 3;
        cv::Mat img_dfdx, img_dfdy;
		cv::Mat img_magnitude, img_direction;
	
		// apply sobel edge detection
		SobelEdgeDetector(img_input, kernel_size, 
						  img_dfdx, img_dfdy,
						  img_magnitude, img_direction);

		// normalise and write output to images
		NormaliseWrite(image_name, "dfdx", 0, img_dfdx);
		NormaliseWrite(image_name, "dfdy", 0, img_dfdy);
		NormaliseWrite(image_name, "magnitude", 0, img_magnitude);
		
		// convert radians to degrees then write 
		cv::Mat img_direction_deg;
		RadToDeg(img_direction, img_direction_deg);
		NormaliseWrite(image_name, "direction", 0, img_direction_deg);
	
		// if thresholding
		if (threshold) {
			// threshold output image
			cv::Mat img_thresholded_magnitude;
			// threshold magnitude and write to file
			Threshold(img_magnitude, threshold_val, 
					  img_thresholded_magnitude);
			NormaliseWrite(image_name, "thresholded", threshold_val, img_thresholded_magnitude);
			// set magnitude image to thresholded version
			img_magnitude = img_thresholded_magnitude.clone();
		}

		// if hough circle transform
		if (hough_circles) {

			// set number of radii to apply
			const int r_size = (max_r - min_r) / r_step;

			// create vector of hough spaces
			std::vector<cv::Mat> hough_space_circles;

			// perform hough circle transform on magnitudes
			GetHoughSpaceCircles(img_magnitude, r_size, min_r, max_r, 
					  			 r_step, t_step, hough_space_circles);

			// normalise and write each hough space image
			// for (int r=0; r<r_size; r++) {
			//	   NormaliseWrite(image_name, "hough_circles_radius", 
			//                    min_r+(r*r_step), hough_space_circles[r]);
			// }

			// // sum hough spaces and write
			// cv::Mat hough_space_circles_summed;
			// SumHoughSpaceCircles(hough_space_circles, hough_space_circles_summed);		 
			// NormaliseWrite(image_name, "hough_circles_summed", 0, hough_space_circles_summed);

			double hough_threshold = 100;

			// find circle centres
			std::vector<pos> hough_circle_locs;
			findHoughCircles(hough_space_circles, hough_threshold,
						  	 hough_circle_locs);

			// draw circles over image
			// drawCircles
			
		}
	
	}

	std::cout << "\nComplete!\n" << std::endl;
	return 0;
}
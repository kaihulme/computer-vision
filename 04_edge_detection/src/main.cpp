#include <include/utils.h>
#include <include/gaussianBlur.h>
#include <include/sobelEdges.h>
#include <include/houghCircles.h>
#include <include/circleDetector.h>
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
	int threshold_h;

	// get image, set argument variables and return -1 if invalid
	int pass = ArgsHandler(argc, argv, img_input, image_name,
                           sobel, hough_circles, 
                           threshold, gaussian,
	                       gaussian_val, threshold_val,
	                       min_r, max_r,
	                       r_step, t_step,
						   threshold_h);
	if (pass == -1) return -1;

	// keep original copy
	const string img_loc = "resources/" + image_name + ".png";
	cv::Mat img_original = imread(img_loc, 1);

	// apply gaussian filter
	if (gaussian) { 
		cv::Mat img_gaussian;
		GaussianBlur(img_input, gaussian_val, img_gaussian);
		NormaliseWrite(img_gaussian, image_name, "gaussian", 0);
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
		NormaliseWrite(img_dfdx, image_name, "dfdx", 0);
		NormaliseWrite(img_dfdy, image_name, "dfdy", 0);
		NormaliseWrite(img_magnitude, image_name, "magnitude", 0);
		
		// convert radians to degrees then write 
		cv::Mat img_direction_deg;
		RadToDeg(img_direction, img_direction_deg);
		NormaliseWrite(img_direction_deg, image_name, "direction", 0);
	
		// if thresholding
		if (threshold) {

			// threshold output image
			cv::Mat img_thresholded_magnitude;

			// threshold magnitude and write to file
			Threshold(img_magnitude, threshold_val, 
					  img_thresholded_magnitude);
			NormaliseWrite(img_thresholded_magnitude, image_name, 
						  "thresholded_magnitude", threshold_val);

			// set magnitude image to thresholded version
			img_magnitude = img_thresholded_magnitude.clone();
			std::cout << "\nGradient magnitude thresholding complete!" << std::endl;

		}

		// if hough circle transform
		if (hough_circles) {

			// set number of radii to apply
			const int r_size = (max_r - min_r) / r_step + 1;

			// create vector of hough spaces
			std::vector<cv::Mat> hough_space;

			// perform hough circle transform on magnitudes
			GetHoughSpaceCircles(img_magnitude, r_size, min_r, max_r, 
					  			 r_step, t_step, hough_space);

			// // normalise and write each hough space image
			// for (int r=0; r<r_size; r++) {
			// 	NormaliseWrite(image_name, "hough_space_r", 
			// 				   min_r+(r*r_step), hough_space[r]);
			// }

			// // sum hough spaces and write
			cv::Mat hough_space_sum;
			SumHoughSpaceCircles(hough_space, hough_space_sum);		 
			NormaliseWrite(hough_space_sum, image_name, "hough_space_sum", 0);

			double sum_threshold = r_size * threshold_h * 0.6;

			// threshold sum
			cv::Mat thresholded_sum;
			Threshold(hough_space_sum, sum_threshold, thresholded_sum);
			NormaliseWrite(thresholded_sum, image_name, "hough_space_sum_thresholded", 0);

			std::vector<cv::Mat> thresholded_hough_space;
			ThresholdHoughSpace(hough_space, threshold_h,
								thresholded_hough_space);

			std::cout << "\nFinding circles..." << std::endl;

			// find circles
			std::vector<circle_t> circles = FindCircles(thresholded_hough_space, 
														thresholded_sum,
														r_size, min_r, r_step);

			// draw circles over image
			DrawCircles(img_original, circles, image_name);
			
		}
	
	}

	std::cout << "\nComplete!\n" << std::endl;
	return 0;
}
#include <include/utils.h>
#include <include/gaussianBlur.h>
#include <include/sobelEdges.h>
#include <include/houghCircles.h>

int main(int argc, char* argv[]) {
	
	// check arg count
	if (argc < 2)  { printf("\nError: image not specified!\n\n");     return -1; }
	
	if (!strcmp(argv[1], "?")) { ArgsHelper(); return -1; }
	else if (argc == 2) { printf("\nError: operation not specified!\n\n"); return -1; }

	// get image location
	const string img_arg = argv[1];
	const string img_loc = "resources/" + img_arg + ".png";
	
	// read image data
 	const Mat image = imread(img_loc, 1);
 	if(!image.data ) {printf("\nError: image not found! ('?' for help)\n\n"); return -1; } 

	// convert to grey
 	Mat img_grey;
 	cvtColor(image, img_grey, CV_BGR2GRAY);
	Mat img_input = img_grey;

	// values for flag handling (with defaults)
	bool sobel, hough_circles, threshold, gaussian = false;
	int gaussian_val = 3; int threshold_val = 100;
	int min_r = 5; int max_r = 50;
	int r_step = 1; int t_step = 1;
	
	// handle flags and set bools
	for (int i=2; i<argc; i++) {

		// if -s apply sobel edge detection
		if (!strcmp(argv[i], "-s") && !sobel) sobel = true;

		// if -h [x][y][z] apply hough transform circles with radii x->y step z
		else if (!strcmp(argv[i], "-h") && !hough_circles) { 
			try {
				min_r  = std::stoi(argv[i+1]); i++;
				max_r  = std::stoi(argv[i+1]); i++;
				r_step = std::stoi(argv[i+1]); i++;
				t_step = std::stoi(argv[i+1]); i++;
			}
			catch (std::exception const &e) {}
			hough_circles = true;
			// i+=4;
		}

		// if -g [x] apply gaussian blur with specified kernel size
		else if (!strcmp(argv[i], "-g") && !gaussian)  {
			try { gaussian_val = std::stoi(argv[i+1]); i++; }
			catch (std::exception const &e) {}
			gaussian = true;
		}

		// if -t [x] apply thresholding to gradient magnitudes at specified value
		else if (!strcmp(argv[i], "-t") && !threshold) {
			try { threshold_val = std::stoi(argv[i+1]); i++; }
			catch (std::exception const &e) {}
			threshold = true;
		}
		
		// unrecognised argument
		else { std::cout << "\nError: check your flags! ('?' for help)\n" << std::endl; return -1; }
	}
	
	if      (threshold && !sobel)     { printf("\nError: magnitude threshold requires sobel edge detection (-s)!\n\n"); return -1; }
	else if (hough_circles && !sobel) { printf("\nError: hough transform requires sobel edge detection (-s)!\n\n");     return -1; }

	// apply gaussian filter
	if (gaussian) { 
		GaussianBlur(img_grey, gaussian_val, img_input);
		NormaliseWrite(img_arg, "gaussian", gaussian_val, img_input);
	}
	
	// sobel edge detector
    if (sobel) {
	
		// set kernel size and output matrices
        const int kernel_size = 3;
        Mat img_dfdx, img_dfdy;
		Mat img_magnitude, img_direction;
	
		// apply sobel edge detection
		SobelEdgeDetector(img_input, kernel_size, 
						  img_dfdx, img_dfdy,
						  img_magnitude, img_direction);

		// normalise and write output to images
		NormaliseWrite(img_arg, "dfdx", 0, img_dfdx);
		NormaliseWrite(img_arg, "dfdy", 0, img_dfdy);
		NormaliseWrite(img_arg, "magnitude", 0, img_magnitude);
		
		// convert radians to degrees before 
		Mat img_direction_deg;
		RadToDeg(img_direction, img_direction_deg);
		NormaliseWrite(img_arg, "direction", 0, img_direction_deg);
	
		// if thresholding
		if (threshold) {
			// threshold output image
			Mat img_thresholded_magnitude;
			// threshold magnitude and write to file
			Threshold(img_magnitude, threshold_val, 
					  img_thresholded_magnitude);
			NormaliseWrite(img_arg, "thresholded", threshold_val, img_thresholded_magnitude);
			// set magnitude image to thresholded version
			img_magnitude = img_thresholded_magnitude;
		}
		// if hough circle transform
		if (hough_circles) {
			// set number of radii to apply
			const int r_size = (max_r - min_r) / r_step;
			// create vector of hough spaces
			std::vector<cv::Mat> hough_circles_space;
			// perform hough circle transform on magnitudes
			HoughTransformCircles(img_magnitude, r_size, min_r, max_r, 
					  			  r_step, t_step, hough_circles_space);
			// normalise and write each hough space image
			for (int r=0; r<r_size; r++) NormaliseWrite(img_arg, "hough_circles_radius", 
														(min_r+(r*r_step)), hough_circles_space[r]);
		}
	
	}

	std::cout << "\nComplete!\n" << std::endl;
	return 0;
}
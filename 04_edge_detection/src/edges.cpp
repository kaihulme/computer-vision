#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>    

using namespace cv;

void SobelEdgeDetector(const cv::Mat &input, const int size, 
					   cv::Mat &dfdx_output, 
					   cv::Mat &dfdy_output,
					   cv::Mat &maginitude_output, 
					   cv::Mat &direction_output);

void HoughTransformCircles(const cv::Mat &input, int r_size,
				     	   int min_r, int max_r, int step_size,
				     	   std::vector<cv::Mat> &hough_circles_space);

void GaussianBlur(const cv::Mat &input, const int size,
	              cv::Mat &gaussian_output);

void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output);

void NormalisePixels(cv::Mat &input, cv::Mat &output);

void NormaliseWrite(string img_name, string out_type, 
					int arg, cv::Mat output);

void RadToDeg(cv::Mat &input, cv::Mat &output);

int main(int argc, char* argv[]) {
	
	// check arg count
	if      (argc < 2)  { printf("\nError: image not specified!\n\n");     return -1; }
	else if (argc == 2) { printf("\nError: operation not specified!\n\n"); return -1; }

	// get image location
	const string img_arg = argv[1];
	const string img_loc = "resources/" + img_arg + ".png";
	
	// read image data
 	const Mat image = imread(img_loc, 1);
 	if(!image.data ) {
   		printf("\nError: image not found!\n\n");
   		return -1;
 	}
	
	// create grey image
 	Mat img_grey;
 	cvtColor(image, img_grey, CV_BGR2GRAY);
	Mat img_input = img_grey;

	// values for flag handling
	bool sobel, hough_circles, threshold, gaussian = false;
	int gaussian_val, threshold_val = 0;
	
	// handle flags and set bools
	for (int i=2; i<argc; i++) {
		if      (!strcmp(argv[i], "-s") && !sobel) {
			sobel = true;
		}
		else if (!strcmp(argv[i], "-h") && !hough_circles) {
			hough_circles = true;
		}
		else if (!strcmp(argv[i], "-t") && !threshold) {
			try {
				threshold_val = std::stoi(argv[i+1]);
				threshold = true;
				i++;
			} catch (std::exception const &e) {
				std::cout << "\nError: threshold value not specified\n" << std::endl; 
				return -1;
			}
		} 
		else if (!strcmp(argv[i], "-g") && !gaussian)  {
			try {
				gaussian_val = std::stoi(argv[i+1]);
				gaussian = true;
				i++;
			} catch (std::exception const &e) {
				std::cout << "\nError: kernel size not specified\n" << std::endl; 
				return -1;
			} 
		}
		else { 
			std::cout << "\nError: check your flags (-s -t [threshold_value] -g [kernel_size])\n" << std::endl; 
			return -1; 
		}
	}
	
	if (threshold && !sobel) { printf("\nError: cannot threshold magnitudes without appling edge detection (-s)!\n\n"); return -1; }

	// apply gaussian filter
	if (gaussian) { 
		GaussianBlur(img_grey, gaussian_val, img_input);
		const string gaussian_out = "out/" + img_arg + "_gaussian.jpg";
		cv::imwrite(gaussian_out, img_input);
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

		// NormaliseAngles(img_direction, img_norm_direction);
		// const string direction_norm_out = "out/" + img_arg + "_direction.jpg";
		// cv::imwrite(direction_norm_out, img_norm_direction);

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
			// min & max circle radius and step size
			const int min_r     = 30;
			const int max_r     = 50;
			const int step_size = 5;
			const int r_size = (max_r - min_r) / step_size;
			// create 3d hough space vector where each 2d slice represents a circle with radius r
			std::vector<cv::Mat> hough_circles_space;
			// perform hough circle transform on magnitudes
			HoughTransformCircles(img_magnitude, r_size, min_r, max_r, 
					  			  step_size, hough_circles_space);
			// normalise and write each hough space image
			for (int r=0; r<r_size; r++) NormaliseWrite(img_arg, "hough_circles_radius", 
														(min_r+(r*step_size)), hough_circles_space[r]);
		}
	
	}

	std::cout << "\nComplete!\n" << std::endl;
	return 0;
}

void SobelEdgeDetector(const cv::Mat &input, const int size, 
					   cv::Mat &dfdx_output, 
					   cv::Mat &dfdy_output,
					   cv::Mat &magnitude_output, 
					   cv::Mat &direction_output) {
	
	// intialise the output using the input
	dfdx_output.create(input.size(), cv::DataType<double>::type);
	dfdy_output.create(input.size(), cv::DataType<double>::type);
	magnitude_output.create(input.size(), cv::DataType<double>::type);
	direction_output.create(input.size(), cv::DataType<double>::type);
	
	// df/fx kernel
	double dfdx_kernel_vals[3][3] = {{-1, 0, 1},
						   			 {-1, 0, 1},
									 {-1, 0, 1}};
    // df/dy kernel
    double dfdy_kernel_vals[3][3] = {{-1,-1,-1},
									 { 0, 0, 0},
									 { 1, 1, 1}};
	
	// create opencv matrix from values
	cv::Mat dfdx_kernel = cv::Mat(size, size, cv::DataType<double>::type, dfdx_kernel_vals);
    cv::Mat dfdy_kernel = cv::Mat(size, size, cv::DataType<double>::type, dfdy_kernel_vals);
	
	// set kernel radius for padding and convolution
	const int r_x = (size-1)/2;
	const int r_y = (size-1)/2;
	
	// replicate edge at border to allow edge convolutions
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   r_x, r_x, r_y, r_y,
					   cv::BORDER_REPLICATE);
	
	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {
	
			// sum for matrix multiplication
			double dfdx_sum = 0.0;
            double dfdy_sum = 0.0;
	
			// for each kernel value
			for(int m=-r_x; m<=r_x; m++) {
				for(int n=-r_y; n<=r_y; n++ ) {
					// correct image and kernel indices
					int img_i    = i + m + r_x;
					int img_j    = j + n + r_y;
					int kernel_i = m + r_x;
					int kernel_j = n + r_y;
					// get the image pixel value and kernel value
					double img_val = (double) padded_input.at<uchar>(img_i, img_j);
					double dfdx_kernel_val = dfdx_kernel.at<double>(kernel_i, kernel_j);
                    double dfdy_kernel_val = dfdy_kernel.at<double>(kernel_i, kernel_j);		
					// add their product to current sum
					dfdx_sum += img_val * dfdx_kernel_val;
                    dfdy_sum += img_val * dfdy_kernel_val;
				}
			}
    
	        // magnitude: ∇|f(x,y)| = sqrt( (df/dx)^2 + (df/dy)^2 )
			// direction: φ = arctan( (df/dy) / (df/dx) )
			double dfdx_pixel      = dfdx_sum;
			double dfdy_pixel      = dfdy_sum;
            double magnitude_pixel = sqrt((dfdx_sum*dfdx_sum) + (dfdy_sum*dfdy_sum));
			double direction_pixel = atan2(dfdy_sum, dfdx_sum);
	
			// update the image with new pixel value
			dfdx_output.at<double>(i, j)      = dfdx_pixel;
			dfdy_output.at<double>(i, j)      = dfdy_pixel;
			magnitude_output.at<double>(i, j) = magnitude_pixel;
			direction_output.at<double>(i, j) = direction_pixel;
	
		}
	}

}

void HoughTransformCircles(const cv::Mat &input, int r_size,
						   int min_r, int max_r, int step_size,
						   std::vector<cv::Mat> &hough_circles_output) {

	// initalise hough_circles output
	cv::Mat hough_space;
	hough_space.create(input.size(), cv::DataType<double>::type);

	// set current radius to min radius
	int radius = min_r;

	// for each circle size
	for (int r=0; r<r_size; r++) {

		// reset hough space for current radius
		for (int i=0; i<input.rows; i++) {	
			for(int j=0; j<input.cols; j++) {
				hough_space.at<double>(i, j) = 0.0;
			}
		}

		// for each pixel in image
		for (int i=0; i<input.rows; i++) {	
			for(int j=0; j<input.cols; j++) {
				// get gradient magnitude at (i,j)
				double pixel_val = input.at<double>(i,j);
				// if gradient magnitude is not 0
				if (pixel_val > 0) {
					// for circle around point
					for (int theta=0; theta<360; theta+=20){
						// get circle coordinates at and theta
						double x0 = i - (radius * cos(theta*CV_PI/180));
						double y0 = j - (radius * sin(theta*CV_PI/180));
						// if point is in image increment value in hough space
						if (x0>=0 && y0>=0 && x0<input.rows && y0<input.cols) {
							hough_space.at<double>(x0,y0)++;
						}
					}
				}
			}
		}

		// add hough space for circle radius r to vector of spaces
		hough_circles_output.push_back(hough_space.clone());
		radius += step_size;
		std::cout << r+1 << ": computed r=" << radius << std::endl;

	}

}

void GaussianBlur(const cv::Mat &input, const int size, 
				  cv::Mat &gaussian_output) {
	
	// intialise the output using the input
	gaussian_output.create(input.size(), input.type());
	
	// create the gaussian kernel
	cv::Mat kX = cv::getGaussianKernel(size, -1);
	cv::Mat kY = cv::getGaussianKernel(size, -1);
	cv::Mat gaussian_kernel = kX * kY.t();
	
	// get radius of kernel for padding and convolution
	const int r_x = (size-1)/2;
	const int r_y = (size-1)/2;
	
	// scale kernel values for size of gaussian kernel
	for( int m=-r_x; m <=r_x; m++) {
	  	for( int n=-r_y; n<=r_y; n++) {
		   	gaussian_kernel.at<double>(m+r_x, n+r_y) = (double) 1.0/(size*size);
		}
	}
	
	// replicate edge at border to allow edge convolutions
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   r_x, r_x, r_y,r_y,
					   cv::BORDER_REPLICATE);
	
	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {
			// sum for matrix multiplication
			double gaussian_sum = 0.0;
			// for each kernel value
			for(int m=-r_x; m<=r_x; m++) {
				for(int n=-r_y; n<=r_y; n++) {
					// correct image and kernel indices
					int img_i    = i + m + r_x;
					int img_j    = j + n + r_y;
					int kernel_i = m + r_x;
					int kernel_j = n + r_y;
					// get the image pixel value and kernel value
					int img_val                = (int)padded_input.at<uchar>(img_i, img_j);
					double gaussian_kernel_val = gaussian_kernel.at<double>(kernel_i, kernel_j);
					// add their product to current sum
					gaussian_sum += img_val * gaussian_kernel_val;
				}
			}
			// update the image with new pixel value
			gaussian_output.at<uchar>(i, j) = (uchar) gaussian_sum;
		}
	}

}

void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output) {
	
	// intialise the output using the input
	thresholded_output.create(input.size(), input.type());
	
	// for each pixel in input
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {
			// get pixel and set output pixel to 0
			double pixel = input.at<double>(i, j);
			double thresholded_pixel = 0;
			// if pixel exceeds threshold set to 255
			if (pixel > threshold_val) {
				thresholded_pixel = 255;
			}
			// set value in output image
			thresholded_output.at<double>(i, j) = thresholded_pixel;
		}
	}

}

void NormalisePixels(cv::Mat &input, cv::Mat &output) {

	// intialise the output using the input
	output.create(input.size(), cv::DataType<double>::type);

	// get min and max values in input
	double minVal, maxVal; 
	Point minLoc, maxLoc;
	minMaxLoc(input, &minVal, &maxVal, &minLoc, &maxLoc);

	// for each pixel in input
	for (int i=0; i<input.rows; i++) {	
		for (int j=0; j<input.cols; j++) {
			// get current pixel
			double pixel = input.at<double>(i, j);
			// normalise pixel between range 0-255
			double normalised_pixel = 255*((pixel-minVal) / (maxVal-minVal));
			output.at<double>(i, j) = normalised_pixel;
		}
	}

}

void NormaliseWrite(string img_name, string out_type, int arg, cv::Mat output) {

	// normalised output image
	cv::Mat normalised_output;
	NormalisePixels(output, normalised_output);

	// create file name based of image input and output type
	string file_name = "out/" + img_name + "_" + out_type;
	file_name += (arg==0 ? ".jpg" : "_" + std::to_string(arg) + ".jpg");



	// if (arg==0) file_name += ".jpg";
	// else 			file_name += "_" + std::to_string(arg) + ".jpg";




	// write normalised image
	cv::imwrite(file_name, normalised_output);

} 

void RadToDeg(cv::Mat &input, cv::Mat &output) {

	// intialise the output using the input
	output.create(input.size(), cv::DataType<double>::type);

	// convert each radian to degrees
	for (int i=0; i<input.rows; i++) {	
		for (int j=0; j<input.cols; j++) {
			double rad = input.at<double>(i, j);
			output.at<double>(i,j) = (rad >= 0 ? rad : (2*CV_PI + rad)) * 360 / (2*CV_PI); 
		}
	}

}

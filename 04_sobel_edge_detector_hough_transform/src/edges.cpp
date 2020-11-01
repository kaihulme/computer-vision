// header inclusion
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

void GaussianBlur(const cv::Mat &input, const int size,
	              cv::Mat &gaussian_output);

void NormalisePixels(cv::Mat &input);
void NormaliseAngles(cv::Mat &input);

void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output);

// TODO if -t but not -s check if sobel outputs are already in /out
// TODO add function for Convolution(input, kernel, width, height, output)
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
	
	// values for flag handling
	bool sobel, threshold, gaussian = false;
	int gaussian_val, threshold_val = 0;
	
	// handle flags and set bools
	for (int i=2; i<argc; i++) {
		if      (!strcmp(argv[i], "-s") && !sobel) {
			sobel = true;
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
		Mat img_input;
		GaussianBlur(img_grey, gaussian_val, img_input);
		const string gaussian_out = "out/" + img_arg + "_gaussian.jpg";
		cv::imwrite(gaussian_out, img_input);
	}
	else Mat img_input = img_grey;
	
	// sobel edge detector
    if (sobel) {
	
		// set kernel size and output matrices
        const int kernel_size = 3;
        Mat img_dfdx, img_dfdy;
		Mat img_magnitude, img_direction;
	
		// apply sobel edge detection
		SobelEdgeDetector(img_grey, kernel_size, 
						  img_dfdx, img_dfdy,
						  img_magnitude, img_direction);
	
		// set file names
        const string dfdx_out = "out/" + img_arg + "_dfdx.jpg";
		const string dfdy_out = "out/" + img_arg + "_dfdy.jpg";
		const string magnitude_out = "out/" + img_arg + "_magnitude.jpg";
		const string direction_out = "out/" + img_arg + "_direction.jpg";
	
		// write images to files
		cv::imwrite(dfdx_out, img_dfdx);
		cv::imwrite(dfdy_out, img_dfdy);
		cv::imwrite(magnitude_out, img_magnitude);
		cv::imwrite(direction_out, img_direction);
	
		// if thresholding
		if (threshold) {
			// threshold output image
			Mat img_thresholded_magnitude;
			// threshold magnitudes
			Threshold(img_magnitude, threshold_val, 
					  img_thresholded_magnitude);
			// set file name and write image to file
			string threshold_direction_out = "out/" + img_arg + "_thresholded_magnitude.jpg";
			cv::imwrite(threshold_direction_out, img_thresholded_magnitude);
		}
	
	}
	
	// edge detection complete
	std::cout << "...\n\nComplete!\n" << std::endl;
 	
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
	
	// normalise pixel values to range 0-255
	NormalisePixels(dfdy_output);
	NormalisePixels(dfdx_output);
	NormalisePixels(magnitude_output);
	NormaliseAngles(direction_output);
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

void NormalisePixels(cv::Mat &input) {

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
			input.at<double>(i, j) = normalised_pixel;
		}
	}

}

void NormaliseAngles(cv::Mat &input) {

	// set pi constant      
	const double pi = atan(1)*4;
	// get min and max values in input
	double minVal, maxVal; 
	Point minLoc, maxLoc;
	minMaxLoc(input, &minVal, &maxVal, &minLoc, &maxLoc);

	// for rach pixel in input
	for (int i=0; i<input.rows; i++) {	
		for (int j=0; j<input.cols; j++) {
			// get current pixel value
			double pixel = input.at<double>(i, j);
			// if angle is positive
			if (pixel >= 0.0) { // convert to degrees and normalise between range 0-255
				input.at<double>(i, j) = (pixel*255)/(2*pi);
			} // if angle is negative
			else { // make positive then convert to degrees and normalise between range 0-255
				input.at<double>(i, j) = (((2*pi)+pixel)*255)/(2*pi);
			}
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
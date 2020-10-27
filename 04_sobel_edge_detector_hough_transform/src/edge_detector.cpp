// header inclusion
#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>    

using namespace cv;

void SobelEdgeDetector(cv::Mat &input, int size, 
					   cv::Mat &sobel_dfdx_output, 
					   cv::Mat &sobel_dfdy_output,
					   cv::Mat &sobel_maginitude_output, 
					   cv::Mat &sobel_direction_output);

void Threshold(cv::Mat &input, 
			  double threshold_val, 
			  cv::Mat &thresholded_output);

void GaussianBlur(cv::Mat &input, 
				  int size,
	              cv::Mat &blurred_output);

void NormalisePixels(cv::Mat &input);
void NormaliseAngles(cv::Mat &input);

int main( int argc, char** argv ) {

	// get image name
	string usr_img;
	std::cout << "\nPlease enter a image name: ";
  	std::cin >> usr_img;
	const string imageName = "resources/" + usr_img + ".png";

	// read image data
 	Mat image;
 	image = imread( imageName, 1 );
 	if(!image.data ) {
   		printf( "\nERROR: image not found!\n\n" );
   		return -1;
 	}

	// create grey image
 	Mat img_grey;
 	cvtColor( image, img_grey, CV_BGR2GRAY );
	string grey_file = "out/" + usr_img + "_grey.jpg";
	imwrite(grey_file, img_grey);

	// get filter type
	string usr_filter;
	std::cout << "Filter type (sobel (s), hough_transform (h)): ";
	std::cin >> usr_filter;

	// get filter type
	string usr_gaussian;
	std::cout << "Apply gaussian filter first? (y/n): ";
	std::cin >> usr_gaussian;

	if (usr_gaussian.compare("y") == 0) { 
		int usr_kernel_size = 0;
        std::cout << "Kernel size: ";
		std::cin >> usr_kernel_size;
		Mat img_input;
		GaussianBlur(img_grey, usr_kernel_size, img_input);
	}
	else { Mat img_input = img_grey; }

	// sobel edge detector
    if (usr_filter.compare("s") == 0) {
		
        int kernel_size = 3;
        Mat img_sobel_edge_detector_dfdx;
		Mat img_sobel_edge_detector_dfdy;
		Mat img_sobel_edge_detector_magnitude;
		Mat img_sobel_edge_detector_direction;

		SobelEdgeDetector(img_grey, kernel_size, 
						  img_sobel_edge_detector_dfdx,
						  img_sobel_edge_detector_dfdy,
						  img_sobel_edge_detector_magnitude,
						  img_sobel_edge_detector_direction);

        string dfdx_out = "out/" + usr_img + "_sobel_edges_dfdx.jpg";
		string dfdy_out = "out/" + usr_img + "_sobel_edges_dfdy.jpg";
		string magnitude_out = "out/" + usr_img + "_sobel_edges_magnitude.jpg";
		string direction_out = "out/" + usr_img + "_sobel_edges_direction.jpg";

		imwrite(dfdx_out, img_sobel_edge_detector_dfdx);
		imwrite(dfdy_out, img_sobel_edge_detector_dfdy);
		imwrite(magnitude_out, img_sobel_edge_detector_magnitude);
		imwrite(direction_out, img_sobel_edge_detector_direction);
	}

	// hough transform
	else if (usr_filter.compare("h") == 0) {

        int kernel_size = 3;
		Mat img_sobel_edge_detector_dfdx;
		Mat img_sobel_edge_detector_dfdy;
		Mat img_sobel_edge_detector_magnitude;
		Mat img_sobel_edge_detector_direction;

        SobelEdgeDetector(img_grey, kernel_size, 
						  img_sobel_edge_detector_dfdx,
						  img_sobel_edge_detector_dfdy,
						  img_sobel_edge_detector_magnitude,
						  img_sobel_edge_detector_direction);

		string before_threshold_out = "out/" + usr_img + "hough_before_threshold.jpg";
		imwrite(before_threshold_out, img_sobel_edge_detector_magnitude);

		// get threshold value
		Mat img_thresholded_magnitude;
		double threshold_val=0.0;
		std::cout << "Threshold value: ";
		std::cin >> threshold_val;

		Threshold(img_sobel_edge_detector_magnitude, 
				  threshold_val, 
				  img_thresholded_magnitude);

		string threshold_direction_out = "out/" + usr_img + "hough_thresholded.jpg";
		imwrite(threshold_direction_out, img_thresholded_magnitude);
	}

	else { 
		std::cout << "\nInvalid filter\n" << std::endl; 
		return 0;
	}

	std::cout << "...\n\nComplete!\n" << std::endl;

 	return 0;
}

void SobelEdgeDetector(cv::Mat &input, int size, 
					   cv::Mat &sobel_dfdx_output, 
					   cv::Mat &sobel_dfdy_output,
					   cv::Mat &sobel_magnitude_output, 
					   cv::Mat &sobel_direction_output) {
	
	// intialise the output using the input
	sobel_dfdx_output.create(input.size(), cv::DataType<double>::type);
	sobel_dfdy_output.create(input.size(), cv::DataType<double>::type);
	sobel_magnitude_output.create(input.size(), cv::DataType<double>::type);
	sobel_direction_output.create(input.size(), cv::DataType<double>::type);

	double direction_array[input.cols][input.rows];

	// df/fx kernel
	double sobel_dfdx_vals[3][3] = { 
		{-1, 0, 1},
		{-1, 0, 1},
		{-1, 0, 1}
	};
    // df/dy kernel
    double sobel_dfdy_vals[3][3] = { 
		{-1,-1,-1},
		{ 0, 0, 0},
		{ 1, 1, 1}
	};

	// create opencv matrix from values
	cv::Mat sobel_dfdx = cv::Mat(size, size, cv::DataType<double>::type, sobel_dfdx_vals);
    cv::Mat sobel_dfdy = cv::Mat(size, size, cv::DataType<double>::type, sobel_dfdy_vals);

	// set kernel radius for convolution
	const int kernel_r_x = (size-1)/2;
	const int kernel_r_y = (size-1)/2;

	// replicate edge at border to allow edge convolutions
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   kernel_r_x, kernel_r_x, kernel_r_y,kernel_r_y,
					   cv::BORDER_REPLICATE);

	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {

			// sum for matrix multiplication
			double sobel_dfdx_sum = 0.0;
            double sobel_dfdy_sum = 0.0;

			// for each kernel value
			for(int m=-kernel_r_x; m<=kernel_r_x; m++) {
				for(int n=-kernel_r_y; n<=kernel_r_y; n++ ) {
					
					// correct image and kernel indices
					int image_x = i + m + kernel_r_x;
					int image_y = j + n + kernel_r_y;
					int kernel_x = m + kernel_r_x;
					int kernel_y = n + kernel_r_y;

					// get the image pixel and kernel value
					double pixel_val = (double) padded_input.at<uchar>(image_x, image_y);
					double sobel_dfdx_val = sobel_dfdx.at<double>(kernel_x, kernel_y);
                    double sobel_dfdy_val = sobel_dfdy.at<double>(kernel_x, kernel_y);
					
					// add their product to current sum
					sobel_dfdx_sum += pixel_val * sobel_dfdx_val;
                    sobel_dfdy_sum += pixel_val * sobel_dfdy_val;
				}
			}

			double dfdx_pixel = sobel_dfdx_sum;
			double dfdy_pixel = sobel_dfdy_sum;

            // grad(f) = ∇|f(x,y)| = sqrt( (df/dx)^2 + (df/dy)^2 )
            double magnitude_pixel = sqrt((sobel_dfdx_sum*sobel_dfdx_sum) + (sobel_dfdy_sum*sobel_dfdy_sum));
			// φ = arctan( (df/dy) / (df/dx) )
			double direction_pixel = atan2(sobel_dfdy_sum, sobel_dfdx_sum);

			// update the image with new pixel value
			sobel_dfdx_output.at<double>(i, j) = dfdx_pixel;
			sobel_dfdy_output.at<double>(i, j) = dfdy_pixel;
			sobel_magnitude_output.at<double>(i, j) = magnitude_pixel;
			sobel_direction_output.at<double>(i, j) = direction_pixel;
		}
	}

	NormalisePixels(sobel_dfdy_output);
	NormalisePixels(sobel_dfdx_output);
	NormalisePixels(sobel_magnitude_output);
	NormaliseAngles(sobel_direction_output);
}

void GaussianBlur(cv::Mat &input, int size, cv::Mat &blurred_output) {
	
	// intialise the output using the input
	blurred_output.create(input.size(), input.type());

	// create the Gaussian kernel in 1D 
	cv::Mat kX = cv::getGaussianKernel(size, -1);
	cv::Mat kY = cv::getGaussianKernel(size, -1);

	// make it 2D multiply one by the transpose of the other
	cv::Mat kernel = kX * kY.t();

	// get radius of kernel for border padding
	const int kernel_r_x = (size-1)/2;
	const int kernel_r_y = (size-1)/2;

	// set kernel values
	for( int m=-kernel_r_x; m <=kernel_r_x; m++) {
	  	for( int n=-kernel_r_y; n<=kernel_r_y; n++) {
		   	kernel.at<double>(m+kernel_r_x, n+kernel_r_y) = (double) 1.0/(size*size);
		}
	}

	// pad border of image with duplicate pixels for convolution
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   kernel_r_x, kernel_r_x, kernel_r_y,kernel_r_y,
					   cv::BORDER_REPLICATE);

	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {

			// sum for matrix multiplication
			double pixel_sum = 0.0;

			// for each kernel value
			for(int m=-kernel_r_x; m<=kernel_r_x; m++) {
				for(int n=-kernel_r_y; n<=kernel_r_y; n++) {
					
					// correct image and kernel indices
					int image_x = i + m + kernel_r_x;
					int image_y = j + n + kernel_r_y;
					int kernel_x = m + kernel_r_x;
					int kernel_y = n + kernel_r_y;

					// get the image pixel and kernel value
					int pixel_val = (int)padded_input.at<uchar>(image_x, image_y);
					double kernel_val = kernel.at<double>(kernel_x, kernel_y);
					
					// add their product to current sum
					pixel_sum += pixel_val * kernel_val;
				}
			}

			// update the image with new pixel value
			blurred_output.at<uchar>(i, j) = (uchar) pixel_sum;
		}
	}
}

void NormalisePixels(cv::Mat &input) {

	double minVal, maxVal; 
	Point minLoc, maxLoc;
	minMaxLoc(input, &minVal, &maxVal, &minLoc, &maxLoc);

	for (int i=0; i<input.rows; i++) {	
		for (int j=0; j<input.cols; j++) {
			double pixel = input.at<double>(i, j);
			double normalised_pixel = 255*((pixel-minVal) / (maxVal-minVal));
			input.at<double>(i, j) = normalised_pixel;
		}
	}
}

void NormaliseAngles(cv::Mat &input) {

	double minVal, maxVal; 
	Point minLoc, maxLoc;
	minMaxLoc(input, &minVal, &maxVal, &minLoc, &maxLoc);

	for (int i=0; i<input.rows; i++) {	
		for (int j=0; j<input.cols; j++) {

			double pixel = input.at<double>(i, j);
			double normalised_pixel = 0.0;
			double pi = atan(1)*4;

			if (pixel >= 0.0) {
				normalised_pixel = (pixel*255)/(2*pi);
			}
			else {
				normalised_pixel = (((2*pi)+pixel)*255)/(2*pi);
			}

			input.at<double>(i, j) = normalised_pixel;
		}
	}
}

void Threshold(cv::Mat &input, double threshold_val, cv::Mat &thresholded_output) {
	
	// intialise the output using the input
	thresholded_output.create(input.size(), input.type());

	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {

			double pixel = input.at<double>(i, j);
			double thresholded_pixel = 0;
		
			if (pixel > threshold_val) {
				thresholded_pixel = 255;
			}
	
			thresholded_output.at<double>(i, j) = thresholded_pixel;
		}
	}

}
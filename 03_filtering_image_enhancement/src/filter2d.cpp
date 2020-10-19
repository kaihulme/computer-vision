// header inclusion
#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

using namespace cv;

void GaussianBlur(
	cv::Mat &input, 
	int size,
	cv::Mat &blurredOutput);

void Sharpen(
	cv::Mat &input, 
	int size,
	cv::Mat &sharpenedOutput);

void UnsharpMask(
	cv::Mat &input, 
	int size,
	cv::Mat &sharpenedOutput);

int main( int argc, char** argv ) {

	string usr_img;
	std::cout << "\nPlease enter a image name: ";
  	std::cin >> usr_img;

	string usr_filter;
	std::cout << "Filter type (blur, sharpen, unsharpen): ";
	std::cin >> usr_filter;

	const string imageName = "resources/" + usr_img + ".png";

 	Mat image;
 	image = imread( imageName, 1 );

 	if(!image.data ) {
   		printf( "Invalid image\n " );
   		return -1;
 	}

 	// CONVERT COLOUR, BLUR AND SAVE
 	Mat gray_image;
 	cvtColor( image, gray_image, CV_BGR2GRAY );
	imwrite("out/grey.jpg", gray_image);

	// if (filter_type == 0) {
	if (usr_filter.compare("blur") == 0) {
		Mat carBlurred;
		GaussianBlur(gray_image, 23, carBlurred);
		imwrite( "out/blur.jpg", carBlurred );
	}

	// else if (filter_type == 1) {
	else if (usr_filter.compare("sharpen") == 0) {
		Mat carSharpened;
		const int sharpen_iterations = 1;
		for(int i=0; i<sharpen_iterations; i++) {
			Sharpen(gray_image, 5, carSharpened);
		}
		imwrite("out/sharpened.jpg", carSharpened);
	}

	// else if (filter_type == 2) {
	else if (usr_filter.compare("unsharpen") == 0) {		
		Mat carUSharpened;
		UnsharpMask(gray_image, 23, carUSharpened);
		imwrite("out/unsharpmask.jpg", carUSharpened);
	}

	else { 
		std::cout << "Invalid filter" << std::endl; 
		return 0;
	}

	std::cout << "...\n\nFiltering complete!\n" << std::endl;

 	return 0;
}

void GaussianBlur(cv::Mat &input, int size, cv::Mat &blurredOutput) {
	// intialise the output using the input
	blurredOutput.create(input.size(), input.type());

	// create the Gaussian kernel in 1D 
	cv::Mat kX = cv::getGaussianKernel(size, -1);
	cv::Mat kY = cv::getGaussianKernel(size, -1);

	// make it 2D multiply one by the transpose of the other
	cv::Mat kernel = kX * kY.t();

	//CREATING A DIFFERENT IMAGE kernel WILL BE NEEDED
	//TO PERFORM OPERATIONS OTHER THAN GUASSIAN BLUR!!!

	// we need to create a padded version of the input
	// or there will be border effects
	int kernelRadiusX = ( kernel.size[0] - 1 ) / 2;
	int kernelRadiusY = ( kernel.size[1] - 1 ) / 2;

	   // SET KERNEL VALUES
	for( int m = -kernelRadiusX; m <= kernelRadiusX; m++ ) {
	  	for( int n = -kernelRadiusY; n <= kernelRadiusY; n++ ) {
		   	kernel.at<double>(m+ kernelRadiusX, n+ kernelRadiusY) = (double) 1.0/(size*size);
		}
	}

	cv::Mat paddedInput;
	cv::copyMakeBorder( input, paddedInput, 
		                kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
		                cv::BORDER_REPLICATE );

	// now we can do the convoltion
	for ( int i = 0; i < input.rows; i++ ) {	
		for( int j = 0; j < input.cols; j++ ) {

			double sum = 0.0;

			for( int m = -kernelRadiusX; m <= kernelRadiusX; m++ ) {
				for( int n = -kernelRadiusY; n <= kernelRadiusY; n++ ) {
			
					// find the correct indices we are using
					int imagex = i + m + kernelRadiusX;
					int imagey = j + n + kernelRadiusY;
					int kernelx = m + kernelRadiusX;
					int kernely = n + kernelRadiusY;

					// get the values from the padded image and the kernel
					int imageval = ( int ) paddedInput.at<uchar>( imagex, imagey );
					double kernalval = kernel.at<double>( kernelx, kernely );

					// do the multiplication
					sum += imageval * kernalval;							
				}
			}
			// set the output value as the sum of the convolution
			blurredOutput.at<uchar>(i, j) = (uchar) sum;
		}
	}
}

void Sharpen(cv::Mat &input, int size, cv::Mat &sharpenedOutput) {
	
	// intialise the output using the input
	sharpenedOutput.create(input.size(), input.type());

	// temp for fixed 5x5 usharp mask kernel
	// const int kernel_size = 5;

	// unsharp masking kernel
	double kernel_vals[5][5] = { 
		{ 1.0,  4.0,    6.0,  4.0, 1.0 },
		{ 4.0, 16.0,   24.0, 16.0, 4.0 },
		{ 6.0, 24.0, -476.0, 24.0, 6.0 },
		{ 4.0, 16.0,   24.0, 16.0, 4.0 },
		{ 1.0,  4.0,    6.0,  4.0, 1.0 } };

	// normalise kernel
	for (int i=0; i<size; i++) {
		for (int j=0; j<size; j++) {
			kernel_vals[j][i] = kernel_vals[j][i] / -256;
		}
	}

	// create cv matrix kernel from kernel_values
	cv::Mat kernel = cv::Mat(size, size, CV_64F, kernel_vals);

	// std::cout << kernel << std::endl;

	const int kernelRadiusX = ( kernel.size[0] - 1 ) / 2;
	const int kernelRadiusY = ( kernel.size[1] - 1 ) / 2;

	cv::Mat paddedInput;
	cv::copyMakeBorder( input, paddedInput, 
						kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
						cv::BORDER_REPLICATE );

	// now we can do the convoltion
	for ( int i = 0; i < input.rows; i++ ) {	
		for( int j = 0; j < input.cols; j++ ) {

			double sum = 0.0;

			for( int m = -kernelRadiusX; m <= kernelRadiusX; m++ ) {
				for( int n = -kernelRadiusY; n <= kernelRadiusY; n++ ) {
					
					// find the correct indices we are using
					int imagex = i + m + kernelRadiusX;
					int imagey = j + n + kernelRadiusY;
					int kernelx = m + kernelRadiusX;
					int kernely = n + kernelRadiusY;

					// get the values from the padded image and the kernel
					int imageval = ( int ) paddedInput.at<uchar>( imagex, imagey );
					double kernalval = kernel.at<double>( kernelx, kernely );
					
					// do the multiplication
					sum += imageval * kernalval;
				}
			}

			// printf("\npixel before: %d\n", input.at<uchar>(i, j));

			// set the output value as the sum of the convolution
			sharpenedOutput.at<uchar>(i, j) = (uchar) sum;

			// printf("pixel after: %d\n", (uchar) sum);
		}
	}
}

void UnsharpMask(cv::Mat &input, int size, cv::Mat &sharpenedOutput) {
	cv::Mat carGaussianBlurred;
	GaussianBlur(input, size, carGaussianBlurred);
	sharpenedOutput = (2 * input) - carGaussianBlurred;
}